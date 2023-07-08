import { Meteor } from 'meteor/meteor';
import { Email } from 'meteor/email';
import { TAPi18n } from 'meteor/rocketchat:tap-i18n';
import _ from 'underscore';
import s from 'underscore.string';
import juice from 'juice';
import stripHtml from 'string-strip-html';

import { settings } from '../../settings/server';
import { escapeHTML } from '../../../lib/escapeHTML';
import { replaceVariables } from './utils.js';

let contentHeader;
let contentFooter;

let body;
let Settings = {
	get: () => {},
};

// define server language for email translations
// @TODO: change TAPi18n.__ function to use the server language by default
let lng = 'en';
settings.get('Language', (key, value) => {
	lng = value || 'en';
});

export const replacekey = (str, key, value = '') => str.replace(new RegExp(`(\\[${ key }\\]|__${ key }__)`, 'igm'), escapeHTML(value));
export const translate = (str) => replaceVariables(str, (match, key) => TAPi18n.__(key, { lng }));
export const replace = function replace(str, data = {}) {
	if (!str) {
		return '';
	}
	const options = {
		Site_Name: Settings.get('Site_Name'),
		Site_URL: Settings.get('Site_Url'),
		Site_URL_Slash: Settings.get('Site_Url').replace(/\/?$/, '/'),
		...data.name && {
			fname: s.strLeft(data.name, ' '),
			lname: s.strRightBack(data.name, ' '),
		},
		...data,
	};
	return Object.entries(options).reduce((ret, [key, value]) => replacekey(ret, key, value), translate(str));
};

const nonEscapeKeys = ['room_path'];

export const replaceEscaped = (str, data = {}) => replace(str, {
	Site_Name: escapeHTML(settings.get('Site_Name')),
	Site_Url: escapeHTML(settings.get('Site_Url')),
	...Object.entries(data).reduce((ret, [key, value]) => {
		ret[key] = nonEscapeKeys.includes(key) ? value : escapeHTML(value);
		return ret;
	}, {}),
});
export const wrap = (html, data = {}) => {
	if (settings.get('email_plain_text_only')) {
		return replace(html, data);
	}

	return replaceEscaped(body.replace('{{body}}', html), data);
};
export const inlinecss = (html) => juice.inlineContent(html, Settings.get('email_style'));
export const getTemplate = (template, fn, escape = true) => {
	let html = '';
	Settings.get(template, (key, value) => {
		html = value || '';
		fn(escape ? inlinecss(html) : html);
	});
	Settings.get('email_style', () => {
		fn(escape ? inlinecss(html) : html);
	});
};
export const getTemplateWrapped = (template, fn) => {
	let html = '';
	const wrapInlineCSS = _.debounce(() => fn(wrap(inlinecss(html))), 100);

	Settings.get('Email_Header', () => html && wrapInlineCSS());
	Settings.get('Email_Footer', () => html && wrapInlineCSS());
	Settings.get('email_style', () => html && wrapInlineCSS());
	Settings.get(template, (key, value) => {
		html = value || '';
		return html && wrapInlineCSS();
	});
};
export const setSettings = (s) => {
	Settings = s;

	getTemplate('Email_Header', (value) => {
		contentHeader = replace(value || '');
		body = inlinecss(`${ contentHeader } {{body}} ${ contentFooter }`);
	}, false);

	getTemplate('Email_Footer', (value) => {
		contentFooter = replace(value || '');
		body = inlinecss(`${ contentHeader } {{body}} ${ contentFooter }`);
	}, false);

	body = inlinecss(`${ contentHeader } {{body}} ${ contentFooter }`);
};

export const rfcMailPatternWithName = /^(?:.*<)?([a-zA-Z0-9.!#$%&'*+\/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*)(?:>?)$/;

export const checkAddressFormat = (from) => rfcMailPatternWithName.test(from);

export const sendNoWrap = ({ to, from, replyTo, subject, html, text, headers }) => {
	if (!checkAddressFormat(to)) {
		return;
	}

	if (!text) {
		text = stripHtml(html).result;
	}

	if (settings.get('email_plain_text_only')) {
		html = undefined;
	}

	Meteor.defer(() => Email.send({ to, from, replyTo, subject, html, text, headers }));
};

export const send = ({ to, from, replyTo, subject, html, text, data, headers }) =>
	sendNoWrap({
		to,
		from,
		replyTo,
		subject: replace(subject, data),
		text: text
			? replace(text, data)
			: stripHtml(replace(html, data)).result,
		html: wrap(html, data),
		headers,
	});

export const checkAddressFormatAndThrow = (from, func) => {
	if (checkAddressFormat(from)) {
		return true;
	}
	throw new Meteor.Error('error-invalid-from-address', 'Invalid from address', {
		function: func,
	});
};

export const getHeader = () => contentHeader;

export const getFooter = () => contentFooter;
