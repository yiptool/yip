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
#include "fmt.h"
#include <cstring>
#include <iostream>
#include <iomanip>

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
	if (m_NewLinePending)
		std::cout << std::endl;
}

void GitProgressPrinter::reset()
{
	memset(&m_FetchProgress, 0, sizeof(m_FetchProgress));
	m_Path = "";
	m_CompletedSteps = 0;
	m_TotalSteps = 0;
	m_NewLinePending = false;
}

void GitProgressPrinter::init(git_clone_options * opts)
{
	init(&opts->checkout_opts);
	opts->remote_callbacks.transfer_progress = &fetchProgress;
	opts->remote_callbacks.payload = this;
}

void GitProgressPrinter::init(git_checkout_options * opts)
{
	reset();
	opts->progress_cb = &checkoutProgress;
	opts->progress_payload = this;
}

void GitProgressPrinter::reportGitProgress()
{
	size_t networkPercent = (100 * m_FetchProgress.received_objects) / m_FetchProgress.total_objects;
	size_t indexPercent = (100 * m_FetchProgress.indexed_objects) / m_FetchProgress.total_objects;
	size_t checkoutPercent = (m_TotalSteps > 0 ? 100 * m_CompletedSteps / m_TotalSteps : 0);
	size_t kbytes = m_FetchProgress.received_bytes / 1024;

	if (m_FetchProgress.received_objects == m_FetchProgress.total_objects)
	{
		std::cout << "Resolving deltas "
			<< m_FetchProgress.indexed_deltas << '/' << m_FetchProgress.total_deltas << "  \r" << std::flush;
		m_NewLinePending = true;
	}
	else
	{
		std::cout
			<< "net "
				<< std::setw(3) << networkPercent << "%% ("
				<< std::setw(4) << kbytes << " kb, "
				<< std::setw(5) << m_FetchProgress.received_objects << '/'
				<< std::setw(5) << m_FetchProgress.total_objects << ")  /  "
			<< "idx "
				<< std::setw(3) << indexPercent << "%% ("
				<< std::setw(5) << m_FetchProgress.indexed_objects << '/'
				<< std::setw(5) << m_FetchProgress.total_objects << ")  /  "
			<< "chk "
				<< std::setw(3) << checkoutPercent << "%% ("
				<< std::setw(4) << m_CompletedSteps << '/'
				<< std::setw(4) << m_TotalSteps << ')'
			<< "   \r"
			<< std::flush;
		m_NewLinePending = true;
	}
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

GitRepositoryPtr GitRepository::openEx(const std::string & path, unsigned flags, const char * ceiling_dirs)
{
	git_repository * repo = nullptr;
	return wrap(repo, git_repository_open_ext(&repo, path.c_str(), flags, ceiling_dirs));
}
