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
#ifndef __5ee804d4037549780b7efcae01037d5e__
#define __5ee804d4037549780b7efcae01037d5e__

#include "../3rdparty/libgit2/include/git2/errors.h"
#include "../3rdparty/libgit2/include/git2/repository.h"
#include "../3rdparty/libgit2/include/git2/clone.h"
#include "../3rdparty/libgit2/include/git2/cred_helpers.h"
#include <memory>
#include <functional>
#include <stdexcept>

class GitRepository;
typedef std::shared_ptr<GitRepository> GitRepositoryPtr;

class GitError : public std::runtime_error
{
public:
	GitError(int error, const git_error * info = giterr_last());
};

class GitProgressPrinter
{
public:
	GitProgressPrinter();
	~GitProgressPrinter();

	void reset();
	void finish();

	void init(git_clone_options * opts);
	void init(git_remote_callbacks * cb);
	void init(git_checkout_options * opts);

	virtual void reportGitFetch(const std::string & url);
	virtual void reportGitClone(const std::string & url);

protected:
	virtual void reportGitProgress();
	virtual void reportGitNewTip(const std::string & oid, const std::string & refname);
	virtual void reportGitUpdatedTip(const std::string & oid1, const std::string & oid2, const std::string & refn);

private:
	git_transfer_progress m_FetchProgress;
	const char * m_Path;
	size_t m_CompletedSteps;
	size_t m_TotalSteps;
	size_t m_ResolvingDeltasPercent;
	bool m_NewLinePending1;
	bool m_NewLinePending2;

	static int fetchProgress(const git_transfer_progress * progress, void * payload);
	static void checkoutProgress(const char * path, size_t cur, size_t tot, void * payload);
	static int updateTips(const char * refname, const git_oid * a, const git_oid * b, void * payload);
};

class GitRepository
{
public:
	GitRepository(git_repository * ptr);
	~GitRepository();

	static GitRepositoryPtr init(const std::string & path, bool bare = false);
	static GitRepositoryPtr initEx(const std::string & path, git_repository_init_options * opts);

	static GitRepositoryPtr clone(const std::string & path, const std::string & url,
		const git_clone_options * opts = nullptr);
	static GitRepositoryPtr clone(const std::string & path, const std::string & url,
		GitProgressPrinter * printer = nullptr);

	static GitRepositoryPtr openEx(const std::string & path, unsigned flags, const char * ceiling_dirs = nullptr);

	std::string path() const;

	void fetch(const char * remoteName = "origin", GitProgressPrinter * printer = nullptr);
	void updateHeadToRemote(const char * remoteName = "origin", GitProgressPrinter * printer = nullptr);

private:
	git_repository * m_Pointer;

	void doFetch(const char * remoteName, GitProgressPrinter * printer,
		const std::function<void(git_remote *)> & afterFetch);
};

#endif
