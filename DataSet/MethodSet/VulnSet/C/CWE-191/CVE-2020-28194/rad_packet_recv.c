int rad_packet_recv(int fd, struct rad_packet_t **p, struct sockaddr_in *addr)
{
	struct rad_packet_t *pack;
	struct rad_attr_t *attr;
	struct rad_dict_attr_t *da;
	struct rad_dict_vendor_t *vendor;
	uint8_t *ptr;
	int n, id, len, vendor_id;
	socklen_t addr_len = sizeof(*addr);

	*p = NULL;

	pack = rad_packet_alloc(0);
	if (!pack)
		return 0;

	//ptr = mmap(NULL, REQ_LENGTH_MAX, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	ptr = mempool_alloc(buf_pool);
	if (ptr == MAP_FAILED) {
		log_emerg("radius:packet: out of memory\n");
		goto out_err;
	}

	pack->buf = ptr;
	clock_gettime(CLOCK_MONOTONIC, &pack->tv);

	while (1) {
		if (addr)
			n = recvfrom(fd, pack->buf, REQ_LENGTH_MAX, 0, addr, &addr_len);
		else
			n = read(fd, pack->buf, REQ_LENGTH_MAX);
		if (n < 0) {
			rad_packet_free(pack);
			if (errno == EAGAIN)
				return 1;

			if (errno != ECONNREFUSED)
				log_ppp_error("radius:packet:read: %s\n", strerror(errno));

			return -1;
		}
		break;
	}

	if (n < 20) {
		log_ppp_warn("radius:packet: short packed received (%i)\n", n);
		goto out_err;
	}

	pack->code = *ptr; ptr++;
	pack->id = *ptr; ptr++;
	pack->len = ntohs(*(uint16_t*)ptr); ptr += 2;

	if (pack->len > n) {
		log_ppp_warn("radius:packet: short packet received %i, expected %i\n", pack->len, n);
		goto out_err;
	}

	ptr += 16;
	n -= 20;

	while (n>0) {
		id = *ptr; ptr++;
		len = *ptr - 2; ptr++;
		if (len < 0) {
			log_ppp_warn("radius:packet short attribute len received\n");
			goto out_err;
		}
		if (2 + len > n) {
			log_ppp_warn("radius:packet: too long attribute received (%i, %i)\n", id, len);
			goto out_err;
		}
		if (id == 26) {
			vendor_id = ntohl(*(uint32_t *)ptr);
			vendor = rad_dict_find_vendor_id(vendor_id);
			if (vendor) {
				ptr += 4;

				if (vendor->tag == 2)
					id = (uint16_t)ntohs(*(uint16_t *)ptr);
				else
					id = *ptr;

				ptr += vendor->tag;

				if (vendor->len == 2)
					len = (uint16_t)ntohs(*(uint16_t *)ptr);
				else
					len = *ptr;

				ptr += vendor->len;

				len -= vendor->tag + vendor->len;

				n -= 4 + vendor->tag + vendor->len;
			} else
				log_ppp_warn("radius:packet: vendor %i not found\n", id);
		} else
			vendor = NULL;
		da = rad_dict_find_attr_id(vendor, id);
		if (da) {
			attr = mempool_alloc(attr_pool);
			if (!attr) {
				log_emerg("radius:packet: out of memory\n");
				goto out_err;
			}
			memset(attr, 0, sizeof(*attr));
			attr->vendor = vendor;
			attr->attr = da;
			attr->len = len;
			attr->raw = ptr;

			if (!da->array) {
				switch (da->type) {
					case ATTR_TYPE_STRING:
						attr->alloc = 1;
						attr->val.string = _malloc(len + 1);
						memcpy(attr->val.string, ptr, len);
						attr->val.string[len] = 0;
						break;
					case ATTR_TYPE_OCTETS:
					case ATTR_TYPE_ETHER:
					case ATTR_TYPE_TLV:
						attr->val.octets = ptr;
						break;
					case ATTR_TYPE_INTEGER:
						if (len != da->size)
							log_ppp_warn("radius:packet: attribute %s has invalid length %i (must be %i)\n", da->name, len, da->size);
					case ATTR_TYPE_DATE:
						if (len == 4)
							attr->val.integer = ntohl(*(uint32_t*)ptr);
						else if (len == 2)
							attr->val.integer = ntohs(*(uint16_t*)ptr);
						else if (len == 1)
							attr->val.integer = *ptr;
						break;
					case ATTR_TYPE_IPADDR:
					case ATTR_TYPE_IFID:
					case ATTR_TYPE_IPV6ADDR:
						memcpy(&attr->val.integer, ptr, len);
						break;
					case ATTR_TYPE_IPV6PREFIX:
						attr->val.ipv6prefix.len = ptr[1];
						memset(&attr->val.ipv6prefix.prefix, 0, sizeof(attr->val.ipv6prefix.prefix));
						memcpy(&attr->val.ipv6prefix.prefix, ptr + 2, len - 2);
						break;
				}
			}
			list_add_tail(&attr->entry, &pack->attrs);
		} else
			log_ppp_warn("radius:packet: unknown attribute received (%i,%i)\n", vendor ? vendor->id : 0, id);
		ptr += len;
		n -= 2 + len;
	}

	*p = pack;

	return 0;

out_err:
	rad_packet_free(pack);
	return 1;
}