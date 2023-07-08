const { clean, override } = abuser(__filename);

function cleanup() {
	clean('.');
	clean('../../helpers/spawn');
}

describe('lib/remote', () => {
	beforeEach(cleanup);
	afterEach(cleanup);

	[
		'git@github.com:repo_owner1/repo-name.git',
		'https://github.com/repo_owner1/repo-name.git',
		'https://username@bitbucket.org/repo_owner1/repo-name.git',
		'git@bitbucket.org:repo_owner1/repo-name.git',
		'git@gitlab.com:repo_owner1/repo-name.git',
		'https://gitlab.com/repo_owner1/repo-name.git',
		'/srv/repo_owner1/repo-name.git',
		'/srv/repo_owner1/repo-name',
		'file:///srv/repo_owner1/repo-name.git',
		'ssh://[username@]repo_owner1/repo-name.git',
		'ssh://[username@]repo_owner1/repo-name',
	].forEach(
		format => it(`should find owner and repo name in ${format}`, async () => {
			override('../../helpers/spawn', () => format);

			const remote = require('.');
			const { owner, name } = await remote();
			expect(owner).to.equal('repo_owner1');
			expect(name).to.equal('repo-name');
		}),
	);

	[
		'file:///repo-name.git',
		'file:///repo-name',
		'repo-name',
	].forEach(
		format => it(
			`Still returns reponame when no owner is found (${format})`,
			async () => {
				override('../../helpers/spawn', async () => format);

				const remote = require('.');
				const { owner, name } = await remote();
				expect(owner).to.equal('');
				expect(name).to.equal('repo-name');
			},
		),
	);
});
