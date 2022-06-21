/*
 * ring buffer tester and benchmark
 *
 * Copyright (C) 2009 Steven Rostedt <srostedt@redhat.com>
 */
#include <linux/ring_buffer.h>
#include <linux/completion.h>
#include <linux/kthread.h>
#include <linux/module.h>
<<<<<<< HEAD
#include <linux/time.h>
=======
#include <linux/ktime.h>
>>>>>>> v4.9.227
#include <asm/local.h>

struct rb_page {
	u64		ts;
	local_t		commit;
	char		data[4080];
};

/* run time and sleep time in seconds */
<<<<<<< HEAD
#define RUN_TIME	10
=======
#define RUN_TIME	10ULL
>>>>>>> v4.9.227
#define SLEEP_TIME	10

/* number of events for writer to wake up the reader */
static int wakeup_interval = 100;

static int reader_finish;
<<<<<<< HEAD
static struct completion read_start;
static struct completion read_done;
=======
static DECLARE_COMPLETION(read_start);
static DECLARE_COMPLETION(read_done);
>>>>>>> v4.9.227

static struct ring_buffer *buffer;
static struct task_struct *producer;
static struct task_struct *consumer;
static unsigned long read;

<<<<<<< HEAD
static int disable_reader;
module_param(disable_reader, uint, 0644);
MODULE_PARM_DESC(disable_reader, "only run producer");

static int write_iteration = 50;
=======
static unsigned int disable_reader;
module_param(disable_reader, uint, 0644);
MODULE_PARM_DESC(disable_reader, "only run producer");

static unsigned int write_iteration = 50;
>>>>>>> v4.9.227
module_param(write_iteration, uint, 0644);
MODULE_PARM_DESC(write_iteration, "# of writes between timestamp readings");

static int producer_nice = MAX_NICE;
static int consumer_nice = MAX_NICE;

static int producer_fifo = -1;
static int consumer_fifo = -1;

<<<<<<< HEAD
module_param(producer_nice, uint, 0644);
MODULE_PARM_DESC(producer_nice, "nice prio for producer");

module_param(consumer_nice, uint, 0644);
MODULE_PARM_DESC(consumer_nice, "nice prio for consumer");

module_param(producer_fifo, uint, 0644);
MODULE_PARM_DESC(producer_fifo, "fifo prio for producer");

module_param(consumer_fifo, uint, 0644);
=======
module_param(producer_nice, int, 0644);
MODULE_PARM_DESC(producer_nice, "nice prio for producer");

module_param(consumer_nice, int, 0644);
MODULE_PARM_DESC(consumer_nice, "nice prio for consumer");

module_param(producer_fifo, int, 0644);
MODULE_PARM_DESC(producer_fifo, "fifo prio for producer");

module_param(consumer_fifo, int, 0644);
>>>>>>> v4.9.227
MODULE_PARM_DESC(consumer_fifo, "fifo prio for consumer");

static int read_events;

<<<<<<< HEAD
static int kill_test;

#define KILL_TEST()				\
	do {					\
		if (!kill_test) {		\
			kill_test = 1;		\
=======
static int test_error;

#define TEST_ERROR()				\
	do {					\
		if (!test_error) {		\
			test_error = 1;		\
>>>>>>> v4.9.227
			WARN_ON(1);		\
		}				\
	} while (0)

enum event_status {
	EVENT_FOUND,
	EVENT_DROPPED,
};

<<<<<<< HEAD
=======
static bool break_test(void)
{
	return test_error || kthread_should_stop();
}

>>>>>>> v4.9.227
static enum event_status read_event(int cpu)
{
	struct ring_buffer_event *event;
	int *entry;
	u64 ts;

	event = ring_buffer_consume(buffer, cpu, &ts, NULL);
	if (!event)
		return EVENT_DROPPED;

	entry = ring_buffer_event_data(event);
	if (*entry != cpu) {
<<<<<<< HEAD
		KILL_TEST();
=======
		TEST_ERROR();
>>>>>>> v4.9.227
		return EVENT_DROPPED;
	}

	read++;
	return EVENT_FOUND;
}

static enum event_status read_page(int cpu)
{
	struct ring_buffer_event *event;
	struct rb_page *rpage;
	unsigned long commit;
	void *bpage;
	int *entry;
	int ret;
	int inc;
	int i;

	bpage = ring_buffer_alloc_read_page(buffer, cpu);
	if (!bpage)
		return EVENT_DROPPED;

	ret = ring_buffer_read_page(buffer, &bpage, PAGE_SIZE, cpu, 1);
	if (ret >= 0) {
		rpage = bpage;
		/* The commit may have missed event flags set, clear them */
		commit = local_read(&rpage->commit) & 0xfffff;
<<<<<<< HEAD
		for (i = 0; i < commit && !kill_test; i += inc) {

			if (i >= (PAGE_SIZE - offsetof(struct rb_page, data))) {
				KILL_TEST();
=======
		for (i = 0; i < commit && !test_error ; i += inc) {

			if (i >= (PAGE_SIZE - offsetof(struct rb_page, data))) {
				TEST_ERROR();
>>>>>>> v4.9.227
				break;
			}

			inc = -1;
			event = (void *)&rpage->data[i];
			switch (event->type_len) {
			case RINGBUF_TYPE_PADDING:
				/* failed writes may be discarded events */
				if (!event->time_delta)
<<<<<<< HEAD
					KILL_TEST();
=======
					TEST_ERROR();
>>>>>>> v4.9.227
				inc = event->array[0] + 4;
				break;
			case RINGBUF_TYPE_TIME_EXTEND:
				inc = 8;
				break;
			case 0:
				entry = ring_buffer_event_data(event);
				if (*entry != cpu) {
<<<<<<< HEAD
					KILL_TEST();
=======
					TEST_ERROR();
>>>>>>> v4.9.227
					break;
				}
				read++;
				if (!event->array[0]) {
<<<<<<< HEAD
					KILL_TEST();
=======
					TEST_ERROR();
>>>>>>> v4.9.227
					break;
				}
				inc = event->array[0] + 4;
				break;
			default:
				entry = ring_buffer_event_data(event);
				if (*entry != cpu) {
<<<<<<< HEAD
					KILL_TEST();
=======
					TEST_ERROR();
>>>>>>> v4.9.227
					break;
				}
				read++;
				inc = ((event->type_len + 1) * 4);
			}
<<<<<<< HEAD
			if (kill_test)
				break;

			if (inc <= 0) {
				KILL_TEST();
=======
			if (test_error)
				break;

			if (inc <= 0) {
				TEST_ERROR();
>>>>>>> v4.9.227
				break;
			}
		}
	}
	ring_buffer_free_read_page(buffer, bpage);

	if (ret < 0)
		return EVENT_DROPPED;
	return EVENT_FOUND;
}

static void ring_buffer_consumer(void)
{
	/* toggle between reading pages and events */
	read_events ^= 1;

	read = 0;
<<<<<<< HEAD
	while (!reader_finish && !kill_test) {
		int found;

		do {
=======
	/*
	 * Continue running until the producer specifically asks to stop
	 * and is ready for the completion.
	 */
	while (!READ_ONCE(reader_finish)) {
		int found = 1;

		while (found && !test_error) {
>>>>>>> v4.9.227
			int cpu;

			found = 0;
			for_each_online_cpu(cpu) {
				enum event_status stat;

				if (read_events)
					stat = read_event(cpu);
				else
					stat = read_page(cpu);

<<<<<<< HEAD
				if (kill_test)
					break;
				if (stat == EVENT_FOUND)
					found = 1;
			}
		} while (found && !kill_test);

=======
				if (test_error)
					break;

				if (stat == EVENT_FOUND)
					found = 1;

			}
		}

		/* Wait till the producer wakes us up when there is more data
		 * available or when the producer wants us to finish reading.
		 */
>>>>>>> v4.9.227
		set_current_state(TASK_INTERRUPTIBLE);
		if (reader_finish)
			break;

		schedule();
	}
<<<<<<< HEAD
=======
	__set_current_state(TASK_RUNNING);
>>>>>>> v4.9.227
	reader_finish = 0;
	complete(&read_done);
}

static void ring_buffer_producer(void)
{
<<<<<<< HEAD
	struct timeval start_tv;
	struct timeval end_tv;
=======
	ktime_t start_time, end_time, timeout;
>>>>>>> v4.9.227
	unsigned long long time;
	unsigned long long entries;
	unsigned long long overruns;
	unsigned long missed = 0;
	unsigned long hit = 0;
	unsigned long avg;
	int cnt = 0;

	/*
	 * Hammer the buffer for 10 secs (this may
	 * make the system stall)
	 */
	trace_printk("Starting ring buffer hammer\n");
<<<<<<< HEAD
	do_gettimeofday(&start_tv);
=======
	start_time = ktime_get();
	timeout = ktime_add_ns(start_time, RUN_TIME * NSEC_PER_SEC);
>>>>>>> v4.9.227
	do {
		struct ring_buffer_event *event;
		int *entry;
		int i;

		for (i = 0; i < write_iteration; i++) {
			event = ring_buffer_lock_reserve(buffer, 10);
			if (!event) {
				missed++;
			} else {
				hit++;
				entry = ring_buffer_event_data(event);
				*entry = smp_processor_id();
				ring_buffer_unlock_commit(buffer, event);
			}
		}
<<<<<<< HEAD
		do_gettimeofday(&end_tv);
=======
		end_time = ktime_get();
>>>>>>> v4.9.227

		cnt++;
		if (consumer && !(cnt % wakeup_interval))
			wake_up_process(consumer);

#ifndef CONFIG_PREEMPT
		/*
		 * If we are a non preempt kernel, the 10 second run will
		 * stop everything while it runs. Instead, we will call
		 * cond_resched and also add any time that was lost by a
		 * rescedule.
		 *
		 * Do a cond resched at the same frequency we would wake up
		 * the reader.
		 */
		if (cnt % wakeup_interval)
			cond_resched();
#endif
<<<<<<< HEAD

	} while (end_tv.tv_sec < (start_tv.tv_sec + RUN_TIME) && !kill_test);
=======
	} while (ktime_before(end_time, timeout) && !break_test());
>>>>>>> v4.9.227
	trace_printk("End ring buffer hammer\n");

	if (consumer) {
		/* Init both completions here to avoid races */
		init_completion(&read_start);
		init_completion(&read_done);
		/* the completions must be visible before the finish var */
		smp_wmb();
		reader_finish = 1;
<<<<<<< HEAD
		/* finish var visible before waking up the consumer */
		smp_wmb();
=======
>>>>>>> v4.9.227
		wake_up_process(consumer);
		wait_for_completion(&read_done);
	}

<<<<<<< HEAD
	time = end_tv.tv_sec - start_tv.tv_sec;
	time *= USEC_PER_SEC;
	time += (long long)((long)end_tv.tv_usec - (long)start_tv.tv_usec);
=======
	time = ktime_us_delta(end_time, start_time);
>>>>>>> v4.9.227

	entries = ring_buffer_entries(buffer);
	overruns = ring_buffer_overruns(buffer);

<<<<<<< HEAD
	if (kill_test)
=======
	if (test_error)
>>>>>>> v4.9.227
		trace_printk("ERROR!\n");

	if (!disable_reader) {
		if (consumer_fifo < 0)
			trace_printk("Running Consumer at nice: %d\n",
				     consumer_nice);
		else
			trace_printk("Running Consumer at SCHED_FIFO %d\n",
				     consumer_fifo);
	}
	if (producer_fifo < 0)
		trace_printk("Running Producer at nice: %d\n",
			     producer_nice);
	else
		trace_printk("Running Producer at SCHED_FIFO %d\n",
			     producer_fifo);

	/* Let the user know that the test is running at low priority */
	if (producer_fifo < 0 && consumer_fifo < 0 &&
	    producer_nice == MAX_NICE && consumer_nice == MAX_NICE)
		trace_printk("WARNING!!! This test is running at lowest priority.\n");

	trace_printk("Time:     %lld (usecs)\n", time);
	trace_printk("Overruns: %lld\n", overruns);
	if (disable_reader)
		trace_printk("Read:     (reader disabled)\n");
	else
		trace_printk("Read:     %ld  (by %s)\n", read,
			read_events ? "events" : "pages");
	trace_printk("Entries:  %lld\n", entries);
	trace_printk("Total:    %lld\n", entries + overruns + read);
	trace_printk("Missed:   %ld\n", missed);
	trace_printk("Hit:      %ld\n", hit);

	/* Convert time from usecs to millisecs */
	do_div(time, USEC_PER_MSEC);
	if (time)
		hit /= (long)time;
	else
		trace_printk("TIME IS ZERO??\n");

	trace_printk("Entries per millisec: %ld\n", hit);

	if (hit) {
		/* Calculate the average time in nanosecs */
		avg = NSEC_PER_MSEC / hit;
		trace_printk("%ld ns per entry\n", avg);
	}

	if (missed) {
		if (time)
			missed /= (long)time;

		trace_printk("Total iterations per millisec: %ld\n",
			     hit + missed);

		/* it is possible that hit + missed will overflow and be zero */
		if (!(hit + missed)) {
			trace_printk("hit + missed overflowed and totalled zero!\n");
			hit--; /* make it non zero */
		}

		/* Caculate the average time in nanosecs */
		avg = NSEC_PER_MSEC / (hit + missed);
		trace_printk("%ld ns per entry\n", avg);
	}
}

static void wait_to_die(void)
{
	set_current_state(TASK_INTERRUPTIBLE);
	while (!kthread_should_stop()) {
		schedule();
		set_current_state(TASK_INTERRUPTIBLE);
	}
	__set_current_state(TASK_RUNNING);
}

static int ring_buffer_consumer_thread(void *arg)
{
<<<<<<< HEAD
	while (!kthread_should_stop() && !kill_test) {
=======
	while (!break_test()) {
>>>>>>> v4.9.227
		complete(&read_start);

		ring_buffer_consumer();

		set_current_state(TASK_INTERRUPTIBLE);
<<<<<<< HEAD
		if (kthread_should_stop() || kill_test)
			break;

=======
		if (break_test())
			break;
>>>>>>> v4.9.227
		schedule();
	}
	__set_current_state(TASK_RUNNING);

<<<<<<< HEAD
	if (kill_test)
=======
	if (!kthread_should_stop())
>>>>>>> v4.9.227
		wait_to_die();

	return 0;
}

static int ring_buffer_producer_thread(void *arg)
{
<<<<<<< HEAD
	init_completion(&read_start);

	while (!kthread_should_stop() && !kill_test) {
		ring_buffer_reset(buffer);

		if (consumer) {
			smp_wmb();
=======
	while (!break_test()) {
		ring_buffer_reset(buffer);

		if (consumer) {
>>>>>>> v4.9.227
			wake_up_process(consumer);
			wait_for_completion(&read_start);
		}

		ring_buffer_producer();
<<<<<<< HEAD

		trace_printk("Sleeping for 10 secs\n");
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ * SLEEP_TIME);
	}

	if (kill_test)
=======
		if (break_test())
			goto out_kill;

		trace_printk("Sleeping for 10 secs\n");
		set_current_state(TASK_INTERRUPTIBLE);
		if (break_test())
			goto out_kill;
		schedule_timeout(HZ * SLEEP_TIME);
	}

out_kill:
	__set_current_state(TASK_RUNNING);
	if (!kthread_should_stop())
>>>>>>> v4.9.227
		wait_to_die();

	return 0;
}

static int __init ring_buffer_benchmark_init(void)
{
	int ret;

	/* make a one meg buffer in overwite mode */
	buffer = ring_buffer_alloc(1000000, RB_FL_OVERWRITE);
	if (!buffer)
		return -ENOMEM;

	if (!disable_reader) {
		consumer = kthread_create(ring_buffer_consumer_thread,
					  NULL, "rb_consumer");
		ret = PTR_ERR(consumer);
		if (IS_ERR(consumer))
			goto out_fail;
	}

	producer = kthread_run(ring_buffer_producer_thread,
			       NULL, "rb_producer");
	ret = PTR_ERR(producer);

	if (IS_ERR(producer))
		goto out_kill;

	/*
	 * Run them as low-prio background tasks by default:
	 */
	if (!disable_reader) {
		if (consumer_fifo >= 0) {
			struct sched_param param = {
				.sched_priority = consumer_fifo
			};
			sched_setscheduler(consumer, SCHED_FIFO, &param);
		} else
			set_user_nice(consumer, consumer_nice);
	}

	if (producer_fifo >= 0) {
		struct sched_param param = {
			.sched_priority = producer_fifo
		};
		sched_setscheduler(producer, SCHED_FIFO, &param);
	} else
		set_user_nice(producer, producer_nice);

	return 0;

 out_kill:
	if (consumer)
		kthread_stop(consumer);

 out_fail:
	ring_buffer_free(buffer);
	return ret;
}

static void __exit ring_buffer_benchmark_exit(void)
{
	kthread_stop(producer);
	if (consumer)
		kthread_stop(consumer);
	ring_buffer_free(buffer);
}

module_init(ring_buffer_benchmark_init);
module_exit(ring_buffer_benchmark_exit);

MODULE_AUTHOR("Steven Rostedt");
MODULE_DESCRIPTION("ring_buffer_benchmark");
MODULE_LICENSE("GPL");
