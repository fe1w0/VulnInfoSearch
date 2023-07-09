void jfs_evict_inode(struct inode *inode)
{
	struct jfs_inode_info *ji = JFS_IP(inode);

	jfs_info("In jfs_evict_inode, inode = 0x%p", inode);

	if (!inode->i_nlink && !is_bad_inode(inode)) {
		dquot_initialize(inode);

		if (JFS_IP(inode)->fileset == FILESYSTEM_I) {
			truncate_inode_pages_final(&inode->i_data);

			if (test_cflag(COMMIT_Freewmap, inode))
				jfs_free_zero_link(inode);

			if (JFS_SBI(inode->i_sb)->ipimap)
				diFree(inode);

			/*
			 * Free the inode from the quota allocation.
			 */
			dquot_free_inode(inode);
		}
	} else {
		truncate_inode_pages_final(&inode->i_data);
	}
	clear_inode(inode);
	dquot_drop(inode);

	BUG_ON(!list_empty(&ji->anon_inode_list));

	spin_lock_irq(&ji->ag_lock);
	if (ji->active_ag != -1) {
		struct bmap *bmap = JFS_SBI(inode->i_sb)->bmap;
		atomic_dec(&bmap->db_active[ji->active_ag]);
		ji->active_ag = -1;
	}
	spin_unlock_irq(&ji->ag_lock);
}