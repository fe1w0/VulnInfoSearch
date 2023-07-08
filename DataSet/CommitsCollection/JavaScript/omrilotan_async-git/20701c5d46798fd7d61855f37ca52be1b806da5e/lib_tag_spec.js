const { clean, override } = abuser(__filename);
const dummy = {};

describe('lib/tag', async() => {
	let gitTag;

	before(() => {
		clean('.');
		override('async-execute', (...args) => dummy.stub(...args));

		gitTag = require('.').bind({
			message: 'this is a message',
			author: 'boaty mcboatface',
			email: 'boaty@boat.face',
		});
	});

	after(() => clean('.'));

	it('Should use a given user', async() => {
		const lines = [];
		dummy.stub = command => lines.push(command);

		await gitTag('1.1.1');
		expect(lines).to.include('git config user.name "boaty mcboatface"');
		expect(lines).to.include('git config user.email "boaty@boat.face"');
	});

	it('Should create a git tag with given message and push it', async() => {
		const lines = [];
		dummy.stub = command => lines.push(command);

		await gitTag('1.1.1');
		expect(lines).to.include('git tag -a "1.1.1" -m "this is a message"');
		expect(lines).to.include('git push origin "refs/tags/1.1.1"');
	});
});
