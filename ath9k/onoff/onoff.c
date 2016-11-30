#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/time.h>

#include "hw.h"

MODULE_LICENSE("Dual CRAPL/GPL");
MODULE_AUTHOR("Iñaki Ucar");
MODULE_DESCRIPTION("ath9k power-save tester");

static bool onoff_enable;
module_param_named(enable, onoff_enable, bool, 0644);
MODULE_PARM_DESC(enable, "Enable power-saving test");

static unsigned int onoff_period = 100000000;
module_param_named(period, onoff_period, uint, 0644);
MODULE_PARM_DESC(period, "Set period (ns)");

static unsigned int onoff_init_s;
module_param_named(init_s, onoff_init_s, uint, 0444);
MODULE_PARM_DESC(init_s, "Init time (s)");

static unsigned int onoff_init_ns;
module_param_named(init_ns, onoff_init_ns, uint, 0444);
MODULE_PARM_DESC(init_ns, "Init time (ns)");

static struct ath_hw *global_ah;
static struct hrtimer htimer;
static ktime_t wait;
static ktime_t period;

static void onoff_set_power_network_sleep(struct ath_hw *ah)
{
  //REG_SET_BIT(ah, AR_STA_ID1, AR_STA_ID1_PWR_SAV);
	/* Set WakeOnInterrupt bit; clear ForceWake bit */
	REG_WRITE(ah, AR_RTC_FORCE_WAKE, AR_RTC_FORCE_WAKE_ON_INT);
}

static void onoff_set_power_awake(struct ath_hw *ah)
{
  REG_SET_BIT(ah, AR_RTC_FORCE_WAKE, AR_RTC_FORCE_WAKE_EN);
  //REG_CLR_BIT(ah, AR_STA_ID1, AR_STA_ID1_PWR_SAV);
}

static enum hrtimer_restart onoff_timer_function(struct hrtimer *timer)
{
  static int state;
	static int first_time;
	struct timespec ts;

	if (!onoff_enable) goto wait;
	if (!first_time) {
		first_time = 1;
		getnstimeofday(&ts);
		onoff_init_s = ts.tv_sec;
		onoff_init_ns = ts.tv_nsec;
	}

	if (!state) {
		onoff_set_power_awake(global_ah);
		state = 1;
	} else {
		onoff_set_power_network_sleep(global_ah);
		state = 0;
	}

	hrtimer_forward_now(timer, period);
	return HRTIMER_RESTART;
wait:
	first_time = 0;
	if (state) {
		onoff_set_power_network_sleep(global_ah);
		state = 0;
	}
	period = ktime_set(0, onoff_period);
	hrtimer_forward_now(timer, wait);
	return HRTIMER_RESTART;
}

void onoff_register_ah(struct ath_hw *ah)
{
	global_ah = ah;
	printk("[onoff] interface registered\n");
	hrtimer_start(&htimer, wait, HRTIMER_MODE_REL);
}
EXPORT_SYMBOL(onoff_register_ah);

static int __init onoff_init(void)
{
	hrtimer_init(&htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	htimer.function = onoff_timer_function;
	period = ktime_set(0, onoff_period);
	wait = ktime_set(1, 0);
  printk("[onoff] module loaded\n");
  return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit onoff_cleanup(void)
{
  hrtimer_cancel(&htimer);
	printk("[onoff] module unloaded\n");
}

module_init(onoff_init);
module_exit(onoff_cleanup);
