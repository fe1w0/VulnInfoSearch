const { clean, override } = abuser(__filename);

describe('lib/unadded', () => {
	let unadded;

	before(() => {
		clean('.');
		override('async-execute', () => `remove '.gitattributes'
add 'apps/post/index.js'
add 'new file.txt'
`);

		unadded = require('.');
	});

	after(() => clean('.'));

	it('should convert dry-run add to a file list', async() => {
		const list = await unadded();
		expect(list).to.deep.equal([
			'.gitattributes',
			'apps/post/index.js',
			'new file.txt',
		]);
	});
});
