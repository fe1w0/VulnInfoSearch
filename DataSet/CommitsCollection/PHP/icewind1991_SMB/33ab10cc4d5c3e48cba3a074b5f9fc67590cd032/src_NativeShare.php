<?php
/**
 * Copyright (c) 2014 Robin Appelman <icewind@owncloud.com>
 * This file is licensed under the Licensed under the MIT license:
 * http://opensource.org/licenses/MIT
 */

namespace Icewind\SMB;

class NativeShare implements IShare {
	/**
	 * @var Server $server
	 */
	private $server;

	/**
	 * @var string $name
	 */
	private $name;

	/**
	 * @var \Icewind\SMB\NativeState $state
	 */
	private $state;

	/**
	 * @param Server $server
	 * @param string $name
	 */
	public function __construct($server, $name) {
		$this->server = $server;
		$this->name = $name;
		$this->state = new NativeState();
	}

	/**
	 * @throws \Icewind\SMB\Exception\ConnectionException
	 * @throws \Icewind\SMB\Exception\AuthenticationException
	 * @throws \Icewind\SMB\Exception\InvalidHostException
	 */
	protected function connect() {
		if ($this->state and $this->state instanceof NativeShare) {
			return;
		}

		$this->state->init($this->server->getWorkgroup(), $this->server->getUser(), $this->server->getPassword());
	}

	/**
	 * Get the name of the share
	 *
	 * @return string
	 */
	public function getName() {
		return $this->name;
	}

	private function buildUrl($path) {
		$url = sprintf('smb://%s/%s', $this->server->getHost(), $this->name);
		if ($path) {
			$path = trim($path, '/');
			$url .= '/';
			$url .= implode('/', array_map('rawurlencode', explode('/', $path)));
		}
		return $url;
	}

	/**
	 * List the content of a remote folder
	 *
	 * @param string $path
	 * @return \Icewind\SMB\IFileInfo[]
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function dir($path) {
		$this->connect();
		$files = array();

		$dh = $this->state->opendir($this->buildUrl($path));
		while ($file = $this->state->readdir($dh)) {
			$name = $file['name'];
			if ($name !== '.' and $name !== '..') {
				$files [] = new NativeFileInfo($this, $path . '/' . $name, $name);
			}
		}

		$this->state->closedir($dh);
		return $files;
	}

	/**
	 * @param string $path
	 * @return \Icewind\SMB\IFileInfo[]
	 */
	public function stat($path) {
		return new NativeFileInfo($this, $path, basename($path), $this->getStat($path));
	}

	public function getStat($path) {
		$this->connect();
		return $this->state->stat($this->buildUrl($path));
	}

	/**
	 * Create a folder on the share
	 *
	 * @param string $path
	 * @return bool
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\AlreadyExistsException
	 */
	public function mkdir($path) {
		$this->connect();
		return $this->state->mkdir($this->buildUrl($path));
	}

	/**
	 * Remove a folder on the share
	 *
	 * @param string $path
	 * @return bool
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function rmdir($path) {
		$this->connect();
		return $this->state->rmdir($this->buildUrl($path));
	}

	/**
	 * Delete a file on the share
	 *
	 * @param string $path
	 * @return bool
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function del($path) {
		$this->connect();
		return $this->state->unlink($this->buildUrl($path));
	}

	/**
	 * Rename a remote file
	 *
	 * @param string $from
	 * @param string $to
	 * @return bool
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\AlreadyExistsException
	 */
	public function rename($from, $to) {
		$this->connect();
		return $this->state->rename($this->buildUrl($from), $this->buildUrl($to));
	}

	/**
	 * Upload a local file
	 *
	 * @param string $source local file
	 * @param string $target remove file
	 * @return bool
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function put($source, $target) {
		$this->connect();
		$sourceHandle = fopen($source, 'rb');
		$targetHandle = $this->state->create($this->buildUrl($target));

		while ($data = fread($sourceHandle, 4096)) {
			$this->state->write($targetHandle, $data);
		}
		$this->state->close($targetHandle);
		return true;
	}

	/**
	 * Download a remote file
	 *
	 * @param string $source remove file
	 * @param string $target local file
	 * @return bool
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function get($source, $target) {
		$this->connect();
		$sourceHandle = $this->state->open($this->buildUrl($source), 'r');
		$targetHandle = fopen($target, 'wb');

		while ($data = $this->state->read($sourceHandle, 4096)) {
			fwrite($targetHandle, $data);
		}
		$this->state->close($sourceHandle);
		return true;
	}

	/**
	 * Open a readable stream top a remote file
	 *
	 * @param string $source
	 * @return resource a read only stream with the contents of the remote file
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function read($source) {
		$this->connect();
		$handle = $this->state->open($this->buildUrl($source), 'r');
		return NativeStream::wrap($this->state, $handle, 'r');
	}

	/**
	 * Open a readable stream top a remote file
	 *
	 * @param string $source
	 * @return resource a read only stream with the contents of the remote file
	 *
	 * @throws \Icewind\SMB\Exception\NotFoundException
	 * @throws \Icewind\SMB\Exception\InvalidTypeException
	 */
	public function write($source) {
		$this->connect();
		$handle = $this->state->create($this->buildUrl($source));
		return NativeStream::wrap($this->state, $handle, 'w');
	}

	/**
	 * Get extended attributes for the path
	 *
	 * @param string $path
	 * @param string $attribute attribute to get the info
	 * @return string the attribute value
	 */
	public function getAttribute($path, $attribute) {
		$this->connect();

		$result = $this->state->getxattr($this->buildUrl($path), $attribute);
		return $result;
	}

	/**
	 * Get extended attributes for the path
	 *
	 * @param string $path
	 * @param string $attribute attribute to get the info
	 * @param mixed $value
	 * @return string the attribute value
	 */
	public function setAttribute($path, $attribute, $value) {
		$this->connect();

		if ($attribute === 'system.dos_attr.mode' and is_int($value)) {
			$value = '0x' . dechex($value);
		}

		return $this->state->setxattr($this->buildUrl($path), $attribute, $value);
	}

	/**
	 * @param string $path
	 * @param int $mode a combination of FileInfo::MODE_READONLY, FileInfo::MODE_ARCHIVE, FileInfo::MODE_SYSTEM and FileInfo::MODE_HIDDEN, FileInfo::NORMAL
	 * @return mixed
	 */
	public function setMode($path, $mode) {
		return $this->setAttribute($path, 'system.dos_attr.mode', $mode);
	}

	public function __destruct() {
		unset($this->state);
	}
}
