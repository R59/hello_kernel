--- a/net/mac80211/sta_info.c
+++ b/net/mac80211/sta_info.c
@@ -2338,5 +2338,9 @@
 {
 	struct sta_info *sta = container_of(pubsta, struct sta_info, sta);
 
+	printk(KERN_INFO "*** I am ieee80211_sta_set_expected_throughput (thr = %d)(sta = %px)(ptr = %px) ***\n",
+		thr, sta, sta->sdata->local->ops->wake_tx_queue);
+
 	sta_update_codel_params(sta, thr);
 }
+EXPORT_SYMBOL(ieee80211_sta_set_expected_throughput);
