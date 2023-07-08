const { clean, override } = abuser(__filename);

const diff = `
		line 1
		line 2
line 3

line 5
`;

describe('lib/list', async() => {
	const exec = stub();
	let list;

	before(() => {
		clean('.');
		override('async-execute', exec);
		exec.returns(diff);
		list = require('.').bind(null, 'some command');
	});
	afterEach(() => exec.resetHistory());
	after(() => clean('.'));

	it('Should call exec with passed command', async() => {
		await list();
		expect(exec).to.have.been.calledWith('some command');
	});
	it('Should hard reset to a given sha', async() => {
		const result = await list();
		expect(result).to.deep.equal([
			'line 1',
			'line 2',
			'line 3',
			'line 5',
		]);
	});
	it('Should return an empty array for empty output', async() => {
		exec.returns('');
		const result = await list();
		expect(result).to.be.an('array');
		expect(result).to.have.lengthOf(0);
	});
	it('Should filter out empty lines', async() => {
		exec.returns(`

		`);
		const result = await list();
		expect(result).to.be.an('array');
		expect(result).to.have.lengthOf(0);
	});
});
