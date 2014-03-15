/* vim: set ai noet ts=4 sw=4 tw=115: */
//
// Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "git.h"
#include "path.h"
#include "fmt.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <algorithm>

/* GitError */

GitError::GitError(int error, const git_error * info)
	: std::runtime_error(fmt() << "git error " << error << (info
		? std::string(fmt() << '/' << info->klass << ": " << info->message) : std::string()))
{
}


/* GitProgressPrinter */

GitProgressPrinter::GitProgressPrinter()
{
	reset();
}

GitProgressPrinter::~GitProgressPrinter()
{
	finish();
}

void GitProgressPrinter::reset()
{
	memset(&m_FetchProgress, 0, sizeof(m_FetchProgress));
	m_Path = "";
	m_CompletedSteps = 0;
	m_TotalSteps = 0;
	m_ResolvingDeltasPercent = static_cast<size_t>(-1);
	m_NewLinePending1 = false;
	m_NewLinePending2 = false;
}

void GitProgressPrinter::finish()
{
	if (m_NewLinePending1 || m_NewLinePending2)
	{
		std::cout << std::endl;
		m_ResolvingDeltasPercent = static_cast<size_t>(-1);
		m_NewLinePending1 = false;
		m_NewLinePending2 = false;
	}
}

void GitProgressPrinter::init(git_clone_options * opts)
{
	init(&opts->checkout_opts);
	init(&opts->remote_callbacks);
}

void GitProgressPrinter::init(git_remote_callbacks * cb)
{
	cb->update_tips = &updateTips;
	cb->transfer_progress = &fetchProgress;
	cb->payload = this;
}

void GitProgressPrinter::init(git_checkout_options * opts)
{
	reset();
	opts->progress_cb = &checkoutProgress;
	opts->progress_payload = this;
}

void GitProgressPrinter::reportGitClone(const std::string & url)
{
	finish();
	std::cout << "git: --- cloning '" << url << "'." << std::endl;
}

void GitProgressPrinter::reportGitFetch(const std::string & url)
{
	finish();
	std::cout << "git: --- fetching '" << url << "'." << std::endl;
}

static size_t calcPercent(size_t cur, size_t total)
{
	if (total == 0)
		return 0;
	if (cur >= total)
		return 100;
	return 100 * cur / total;
}

void GitProgressPrinter::reportGitProgress()
{
	if (m_FetchProgress.indexed_deltas > 0 && m_FetchProgress.received_objects == m_FetchProgress.total_objects)
	{
		size_t percent = calcPercent(m_FetchProgress.indexed_deltas, m_FetchProgress.total_deltas);
		if (percent != m_ResolvingDeltasPercent)
		{
			if (m_NewLinePending2)
			{
				std::cout << std::endl;
				m_NewLinePending2 = false;
			}

			std::cout << "\rgit: resolving deltas: " << percent << "%" << std::flush;

			m_ResolvingDeltasPercent = percent;
			m_NewLinePending1 = true;
		}
	}
	else if (m_FetchProgress.total_objects > 0 || m_FetchProgress.received_bytes > 0 || m_TotalSteps > 0)
	{
		if (m_NewLinePending1)
		{
			std::cout << std::endl;
			m_ResolvingDeltasPercent = static_cast<size_t>(-1);
			m_NewLinePending1 = false;
		}

		const char * pre = "";
		std::stringstream ss;

		if (m_FetchProgress.total_objects > 0)
		{
			size_t cur = std::max(m_FetchProgress.received_objects, m_FetchProgress.indexed_objects);
			size_t total = m_FetchProgress.total_objects;
			ss << pre << "receiving objects: " << calcPercent(cur, total) << "% (" << cur << '/' << total << ")";
			pre = ", ";
		}

		if (m_TotalSteps > 0)
		{
			ss << pre << "checkout: " << calcPercent(m_CompletedSteps, m_TotalSteps) << "% ("
				<< m_CompletedSteps << '/' << m_TotalSteps << ')';
			pre = ", ";
		}

		if (m_FetchProgress.received_bytes > 0)
		{
			char buf[256];
			if (m_FetchProgress.received_bytes < 1048576)
				sprintf(buf, "%u KB", static_cast<unsigned>(m_FetchProgress.received_bytes) / 1024);
			else
				sprintf(buf, "%.1f MB", static_cast<double>(m_FetchProgress.received_bytes) / 1048576.0);
			ss << pre << buf;
		}

		std::cout << "\rgit: " << ss.str() << std::flush;
		m_NewLinePending2 = true;
	}
}

void GitProgressPrinter::reportGitNewTip(const std::string & oid, const std::string & refname)
{
	finish();
	std::cout << "git: [new] " << oid.substr(0, 10) << ' ' << refname << std::endl;
}

void GitProgressPrinter::reportGitUpdatedTip(const std::string & oid1, const std::string & oid2,
	const std::string & refn)
{
	finish();
	std::cout << "git: [upd] " << oid1.substr(0, 10) << ".." << oid2.substr(0, 10) << ' ' << refn << std::endl;
}

int GitProgressPrinter::fetchProgress(const git_transfer_progress * progress, void * payload)
{
	GitProgressPrinter * self = reinterpret_cast<GitProgressPrinter *>(payload);
	self->m_FetchProgress = *progress;
	self->reportGitProgress();
	return 0;
}

void GitProgressPrinter::checkoutProgress(const char * path, size_t cur, size_t tot, void * payload)
{
	GitProgressPrinter * self = reinterpret_cast<GitProgressPrinter *>(payload);
	self->m_Path = path;
	self->m_CompletedSteps = cur;
	self->m_TotalSteps = tot;
	self->reportGitProgress();
}

int GitProgressPrinter::updateTips(const char * refname, const git_oid * a, const git_oid * b, void * payload)
{
	GitProgressPrinter * self = reinterpret_cast<GitProgressPrinter *>(payload);
	char astr[GIT_OID_HEXSZ + 1], bstr[GIT_OID_HEXSZ + 1];

	git_oid_fmt(bstr, b);
	bstr[GIT_OID_HEXSZ] = 0;

	if (git_oid_iszero(a))
		self->reportGitNewTip(bstr, refname);
	else
	{
		git_oid_fmt(astr, a);
		astr[GIT_OID_HEXSZ] = 0;
		self->reportGitUpdatedTip(astr, bstr, refname);
	}

	return 0;
}


/* GitRepository */

GitRepository::GitRepository(git_repository * ptr)
	: m_Pointer(ptr)
{
}

GitRepository::~GitRepository()
{
	git_repository_free(m_Pointer);
}

static GitRepositoryPtr wrap(git_repository *& rep, int error)
{
	try
	{
		if (error < 0 || !rep)
			throw GitError(error);
		return std::make_shared<GitRepository>(rep);
	}
	catch (...)
	{
		if (rep)
			git_repository_free(rep);
		throw;
	}
}

GitRepositoryPtr GitRepository::init(const std::string & path, bool bare)
{
	git_repository * repo = nullptr;
	return wrap(repo, git_repository_init(&repo, path.c_str(), bare));
}

GitRepositoryPtr GitRepository::initEx(const std::string & path, git_repository_init_options * opts)
{
	git_repository * repo = nullptr;
	return wrap(repo, git_repository_init_ext(&repo, path.c_str(), opts));
}

GitRepositoryPtr GitRepository::clone(const std::string & path, const std::string & url,
	const git_clone_options * opts)
{
	git_repository * repo = nullptr;
	return wrap(repo, git_clone(&repo, url.c_str(), path.c_str(), opts));
}

GitRepositoryPtr GitRepository::clone(const std::string & path, const std::string & url,
	GitProgressPrinter * printer)
{
	GitRepositoryPtr repo;

	git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
	checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;

	git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
	clone_opts.checkout_opts = checkout_opts;
	clone_opts.remote_callbacks.credentials = git_cred_userpass;
	clone_opts.ignore_cert_errors = true;

	if (printer)
	{
		printer->init(&clone_opts);
		printer->reportGitClone(url);
	}

	try
	{
		repo = GitRepository::clone(path, url, &clone_opts);
	}
	catch (...)
	{
		if (printer)
			printer->finish();
		throw;
	}

	if (printer)
		printer->finish();

	return repo;
}

GitRepositoryPtr GitRepository::openEx(const std::string & path, unsigned flags, const char * ceiling_dirs)
{
	git_repository * repo = nullptr;
	return wrap(repo, git_repository_open_ext(&repo, path.c_str(), flags, ceiling_dirs));
}

std::string GitRepository::path() const
{
	const char * dir = git_repository_path(m_Pointer);
	if (!dir)
		return std::string();

	size_t len = strlen(dir);

	if (len >= 4 && !memcmp(&dir[len - 4], ".git", 4))
		return std::string(dir, len - 4);

	if (len >= 5 && !memcmp(&dir[len - 5], ".git", 4) && pathIsSeparator(dir[len - 1]))
		return std::string(dir, len - 5);

	return std::string(dir, len);
}

void GitRepository::fetch(const char * remoteName, GitProgressPrinter * printer)
{
	doFetch(remoteName, printer, [](git_remote *){});
}

void GitRepository::updateHeadToRemote(const char * remoteName, GitProgressPrinter * printer)
{
	git_repository * const repo = m_Pointer;
	doFetch(remoteName, printer, [repo, printer](git_remote * remote)
	{
		int error = git_update_head_to_remote(repo, remote, nullptr, nullptr);
		if (error < 0)
			throw GitError(error);

		git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
		checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;

		if (printer)
			printer->init(&checkout_opts);

		error = git_checkout_head(repo, &checkout_opts);
		if (error < 0)
			throw GitError(error);
	});
}

void GitRepository::doFetch(const char * remoteName, GitProgressPrinter * printer,
	const std::function<void(git_remote *)> & afterFetch)
{
	git_remote * remote = nullptr;
	int error = git_remote_load(&remote, m_Pointer, remoteName);
	if (error < 0 || !remote)
		throw GitError(error);

	try
	{
		if (printer)
		{
			const char * url = git_remote_url(remote);
			printer->reportGitFetch(url ? url : "");
		}

		git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
		if (printer)
			printer->init(&callbacks);

		git_remote_set_callbacks(remote, &callbacks);
		git_remote_check_cert(remote, false);

		try
		{
			error = git_remote_fetch(remote, nullptr, nullptr);
			if (error < 0)
				throw GitError(error);

			afterFetch(remote);
		}
		catch (...)
		{
			if (printer)
				printer->finish();
			throw;
		}

		if (printer)
			printer->finish();
	}
	catch (...)
	{
		git_remote_disconnect(remote);
		git_remote_free(remote);
		throw;
	}

	git_remote_disconnect(remote);
	git_remote_free(remote);
}
