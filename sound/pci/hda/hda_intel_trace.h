#undef TRACE_SYSTEM
#define TRACE_SYSTEM hda_intel
#define TRACE_INCLUDE_FILE hda_intel_trace

#if !defined(_TRACE_HDA_INTEL_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_HDA_INTEL_H

#include <linux/tracepoint.h>

<<<<<<< HEAD
struct azx;
struct azx_dev;

TRACE_EVENT(azx_pcm_trigger,

	TP_PROTO(struct azx *chip, struct azx_dev *dev, int cmd),

	TP_ARGS(chip, dev, cmd),

	TP_STRUCT__entry(
		__field( int, card )
		__field( int, idx )
		__field( int, cmd )
	),

	TP_fast_assign(
		__entry->card = (chip)->card->number;
		__entry->idx = (dev)->index;
		__entry->cmd = cmd;
	),

	TP_printk("[%d:%d] cmd=%d", __entry->card, __entry->idx, __entry->cmd)
);

TRACE_EVENT(azx_get_position,

    TP_PROTO(struct azx *chip, struct azx_dev *dev, unsigned int pos, unsigned int delay),

	    TP_ARGS(chip, dev, pos, delay),

	TP_STRUCT__entry(
		__field( int, card )
		__field( int, idx )
		__field( unsigned int, pos )
		__field( unsigned int, delay )
	),

	TP_fast_assign(
		__entry->card = (chip)->card->number;
		__entry->idx = (dev)->index;
		__entry->pos = pos;
		__entry->delay = delay;
	),

	TP_printk("[%d:%d] pos=%u, delay=%u", __entry->card, __entry->idx, __entry->pos, __entry->delay)
);

=======
DECLARE_EVENT_CLASS(hda_pm,
	TP_PROTO(struct azx *chip),

	TP_ARGS(chip),

	TP_STRUCT__entry(
		__field(int, dev_index)
	),

	TP_fast_assign(
		__entry->dev_index = (chip)->dev_index;
	),

	TP_printk("card index: %d", __entry->dev_index)
);

DEFINE_EVENT(hda_pm, azx_suspend,
	TP_PROTO(struct azx *chip),
	TP_ARGS(chip)
);

DEFINE_EVENT(hda_pm, azx_resume,
	TP_PROTO(struct azx *chip),
	TP_ARGS(chip)
);

#ifdef CONFIG_PM
DEFINE_EVENT(hda_pm, azx_runtime_suspend,
	TP_PROTO(struct azx *chip),
	TP_ARGS(chip)
);

DEFINE_EVENT(hda_pm, azx_runtime_resume,
	TP_PROTO(struct azx *chip),
	TP_ARGS(chip)
);
#endif

>>>>>>> v4.9.227
#endif /* _TRACE_HDA_INTEL_H */

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#include <trace/define_trace.h>
