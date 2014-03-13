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

	void init(git_clone_options * opts);
	void init(git_checkout_options * opts);

protected:
	virtual void reportGitProgress();

private:
	git_transfer_progress m_FetchProgress;
	const char * m_Path;
	size_t m_CompletedSteps;
	size_t m_TotalSteps;
	bool m_NewLinePending;

	static int fetchProgress(const git_transfer_progress * progress, void * payload);
	static void checkoutProgress(const char * path, size_t cur, size_t tot, void * payload);
};

class GitRepository
{
public:
	GitRepository(git_repository * ptr);
	~GitRepository();

	static GitRepositoryPtr init(const std::string & path, bool bare = false);
	static GitRepositoryPtr initEx(const std::string & path, git_repository_init_options * opts);

	static GitRepositoryPtr clone(const std::string & path, const std::string & url,
		const git_clone_options * opts = NULL);

	static GitRepositoryPtr openEx(const std::string & path, unsigned flags, const char * ceiling_dirs = nullptr);

private:
	git_repository * m_Pointer;
};

#endif
