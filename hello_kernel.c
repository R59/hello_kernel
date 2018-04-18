#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

#include <linux/netdevice.h>
#include <net/cfg80211.h>
#include <net/mac80211.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("*** module author ***");
MODULE_DESCRIPTION("*** module description ***");

#define DATA_BUF_SIZE 100
/*
static int dfs_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}
*/
struct ieee80211_sta *find_sta(u8 *addr, u32 logging)
{
	struct net_device    *dev = NULL;
	struct wireless_dev *wdev = NULL;
	struct ieee80211_vif *vif = NULL;
	struct ieee80211_sta *sta = NULL;

	read_lock(&dev_base_lock);

	dev = first_net_device(&init_net);
	while (dev) {
		if (logging)
			printk(KERN_INFO " dev: name[%s] MAC[%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX]\n",
				dev->name,
				dev->perm_addr[0], dev->perm_addr[1], dev->perm_addr[2],
				dev->perm_addr[3], dev->perm_addr[4], dev->perm_addr[5]
			);
		wdev = dev->ieee80211_ptr;
		if (wdev)
		{
			if (logging)
				printk(KERN_INFO "  wdev\n");
			vif = wdev_to_ieee80211_vif(wdev);
			if (vif)
			{
				if (logging)
					printk(KERN_INFO "   vif\n");
				sta = ieee80211_find_sta(vif, addr);
				if (sta)
				{
					if (logging)
						printk(KERN_INFO "    sta: MAC[%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX]\n",
							sta->addr[0], sta->addr[1], sta->addr[2],
							sta->addr[3], sta->addr[4], sta->addr[5]
						);
					break;
				}
			}
		}
		dev = next_net_device(dev);
	}

	read_unlock(&dev_base_lock);

	return sta;
}

extern void ieee80211_sta_set_expected_throughput(struct ieee80211_sta *, u32);

void parse_str_and_call_function(char *str)
{
	u8  addr[6];
	u32 thr = 0;
	u32 logging = 0;

	if ( 6 > sscanf(str, "%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX %u %u",
		&addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5], &thr, &logging)
	)
		return;

	if (logging)
		printk(KERN_INFO "Searching for sta with MAC[%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX]\n",
			addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
		);
	struct ieee80211_sta *sta = find_sta(addr_bytes, logging);
	if (sta)
	{
		if (logging)
			printk(KERN_INFO " Calling ieee80211_sta_set_expected_throughput\n");
		ieee80211_sta_set_expected_throughput(sta, thr);
	}
}

static ssize_t dfs_read(struct file *file, char __user *buff, size_t count, loff_t *pos)
{
	return 0;
}

static ssize_t dfs_write(struct file *file, const char __user *buff, size_t count, loff_t *pos)
{
	if (*pos == 0 && count < DATA_BUF_SIZE)
	{
		char data_buf[DATA_BUF_SIZE];
		if (copy_from_user(data_buf, buff, count))
			return -EFAULT;

		data_buf[count] = '\0';

		parse_str_and_call_function(data_buf);
	}

	return count;
}

static struct file_operations file_ops ={
	.owner = THIS_MODULE,
	.open  = NULL,//dfs_open,
	.read  = dfs_read,
	.write = dfs_write,
};

struct dentry *dfs_file;

int init_module(void)
{
	printk(KERN_INFO "Hello kernel\n");

	dfs_file = debugfs_create_file("hello_kernel_file", 0777, NULL, NULL, &file_ops);
	if(!dfs_file)
		return -ENOENT;
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Bye kernel\n");

	debugfs_remove(dfs_file);
}
