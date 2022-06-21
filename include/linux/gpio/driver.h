#ifndef __LINUX_GPIO_DRIVER_H
#define __LINUX_GPIO_DRIVER_H

<<<<<<< HEAD
#include <linux/types.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>

struct device;
=======
#include <linux/device.h>
#include <linux/types.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/lockdep.h>
#include <linux/pinctrl/pinctrl.h>

>>>>>>> v4.9.227
struct gpio_desc;
struct of_phandle_args;
struct device_node;
struct seq_file;
<<<<<<< HEAD
=======
struct gpio_device;
struct module;
>>>>>>> v4.9.227

#ifdef CONFIG_GPIOLIB

/**
<<<<<<< HEAD
 * struct gpio_chip - abstract a GPIO controller
 * @label: for diagnostics
 * @dev: optional device providing the GPIOs
 * @owner: helps prevent removal of modules exporting active GPIOs
 * @list: links gpio_chips together for traversal
=======
 * enum single_ended_mode - mode for single ended operation
 * @LINE_MODE_PUSH_PULL: normal mode for a GPIO line, drive actively high/low
 * @LINE_MODE_OPEN_DRAIN: set line to be open drain
 * @LINE_MODE_OPEN_SOURCE: set line to be open source
 */
enum single_ended_mode {
	LINE_MODE_PUSH_PULL,
	LINE_MODE_OPEN_DRAIN,
	LINE_MODE_OPEN_SOURCE,
};

/**
 * struct gpio_chip - abstract a GPIO controller
 * @label: a functional name for the GPIO device, such as a part
 *	number or the name of the SoC IP-block implementing it.
 * @gpiodev: the internal state holder, opaque struct
 * @parent: optional parent device providing the GPIOs
 * @owner: helps prevent removal of modules exporting active GPIOs
>>>>>>> v4.9.227
 * @request: optional hook for chip-specific activation, such as
 *	enabling module power and clock; may sleep
 * @free: optional hook for chip-specific deactivation, such as
 *	disabling module power and clock; may sleep
 * @get_direction: returns direction for signal "offset", 0=out, 1=in,
 *	(same as GPIOF_DIR_XXX), or negative error
 * @direction_input: configures signal "offset" as input, or returns error
 * @direction_output: configures signal "offset" as output, or returns error
<<<<<<< HEAD
 * @get: returns value for signal "offset"; for output signals this
 *	returns either the value actually sensed, or zero
 * @set: assigns output value for signal "offset"
 * @set_debounce: optional hook for setting debounce time for specified gpio in
 *      interrupt triggered gpio chips
=======
 * @get: returns value for signal "offset", 0=low, 1=high, or negative error
 * @set: assigns output value for signal "offset"
 * @set_multiple: assigns output values for multiple signals defined by "mask"
 * @set_debounce: optional hook for setting debounce time for specified gpio in
 *	interrupt triggered gpio chips
 * @set_single_ended: optional hook for setting a line as open drain, open
 *	source, or non-single ended (restore from open drain/source to normal
 *	push-pull mode) this should be implemented if the hardware supports
 *	open drain or open source settings. The GPIOlib will otherwise try
 *	to emulate open drain/source by not actively driving lines high/low
 *	if a consumer request this. The driver may return -ENOTSUPP if e.g.
 *	it supports just open drain but not open source and is called
 *	with LINE_MODE_OPEN_SOURCE as mode argument.
>>>>>>> v4.9.227
 * @to_irq: optional hook supporting non-static gpio_to_irq() mappings;
 *	implementation may not sleep
 * @dbg_show: optional routine to show contents in debugfs; default code
 *	will be used when this is omitted, but custom code can show extra
 *	state (such as pullup/pulldown configuration).
<<<<<<< HEAD
 * @base: identifies the first GPIO number handled by this chip; or, if
 *	negative during registration, requests dynamic ID allocation.
 * @ngpio: the number of GPIOs handled by this controller; the last GPIO
 *	handled is (base + ngpio - 1).
 * @desc: array of ngpio descriptors. Private.
=======
 * @base: identifies the first GPIO number handled by this chip;
 *	or, if negative during registration, requests dynamic ID allocation.
 *	DEPRECATION: providing anything non-negative and nailing the base
 *	offset of GPIO chips is deprecated. Please pass -1 as base to
 *	let gpiolib select the chip base in all possible cases. We want to
 *	get rid of the static GPIO number space in the long run.
 * @ngpio: the number of GPIOs handled by this controller; the last GPIO
 *	handled is (base + ngpio - 1).
>>>>>>> v4.9.227
 * @names: if set, must be an array of strings to use as alternative
 *      names for the GPIOs in this chip. Any entry in the array
 *      may be NULL if there is no alias for the GPIO, however the
 *      array must be @ngpio entries long.  A name can include a single printk
 *      format specifier for an unsigned int.  It is substituted by the actual
 *      number of the gpio.
 * @can_sleep: flag must be set iff get()/set() methods sleep, as they
 *	must while accessing GPIO expander chips over I2C or SPI. This
 *	implies that if the chip supports IRQs, these IRQs need to be threaded
 *	as the chip access may sleep when e.g. reading out the IRQ status
 *	registers.
<<<<<<< HEAD
 * @exported: flags if the gpiochip is exported for use from sysfs. Private.
 * @irq_not_threaded: flag must be set if @can_sleep is set but the
 *	IRQs don't need to be threaded
=======
 * @irq_not_threaded: flag must be set if @can_sleep is set but the
 *	IRQs don't need to be threaded
 * @read_reg: reader function for generic GPIO
 * @write_reg: writer function for generic GPIO
 * @pin2mask: some generic GPIO controllers work with the big-endian bits
 *	notation, e.g. in a 8-bits register, GPIO7 is the least significant
 *	bit. This callback assigns the right bit mask.
 * @reg_dat: data (in) register for generic GPIO
 * @reg_set: output set register (out=high) for generic GPIO
 * @reg_clk: output clear register (out=low) for generic GPIO
 * @reg_dir: direction setting register for generic GPIO
 * @bgpio_bits: number of register bits used for a generic GPIO i.e.
 *	<register width> * 8
 * @bgpio_lock: used to lock chip->bgpio_data. Also, this is needed to keep
 *	shadowed and real data registers writes together.
 * @bgpio_data:	shadowed data register for generic GPIO to clear/set bits
 *	safely.
 * @bgpio_dir: shadowed direction register for generic GPIO to clear/set
 *	direction safely.
 * @irqchip: GPIO IRQ chip impl, provided by GPIO driver
 * @irqdomain: Interrupt translation domain; responsible for mapping
 *	between GPIO hwirq number and linux irq number
 * @irq_base: first linux IRQ number assigned to GPIO IRQ chip (deprecated)
 * @irq_handler: the irq handler to use (often a predefined irq core function)
 *	for GPIO IRQs, provided by GPIO driver
 * @irq_default_type: default IRQ triggering type applied during GPIO driver
 *	initialization, provided by GPIO driver
 * @irq_parent: GPIO IRQ chip parent/bank linux irq number,
 *	provided by GPIO driver
 * @irq_need_valid_mask: If set core allocates @irq_valid_mask with all
 *	bits set to one
 * @irq_valid_mask: If not %NULL holds bitmask of GPIOs which are valid to
 *	be included in IRQ domain of the chip
 * @lock_key: per GPIO IRQ chip lockdep class
>>>>>>> v4.9.227
 *
 * A gpio_chip can help platforms abstract various sources of GPIOs so
 * they can all be accessed through a common programing interface.
 * Example sources would be SOC controllers, FPGAs, multifunction
 * chips, dedicated GPIO expanders, and so on.
 *
 * Each chip controls a number of signals, identified in method calls
 * by "offset" values in the range 0..(@ngpio - 1).  When those signals
 * are referenced through calls like gpio_get_value(gpio), the offset
 * is calculated by subtracting @base from the gpio number.
 */
struct gpio_chip {
	const char		*label;
<<<<<<< HEAD
	struct device		*dev;
	struct module		*owner;
	struct list_head        list;
=======
	struct gpio_device	*gpiodev;
	struct device		*parent;
	struct module		*owner;
>>>>>>> v4.9.227

	int			(*request)(struct gpio_chip *chip,
						unsigned offset);
	void			(*free)(struct gpio_chip *chip,
						unsigned offset);
	int			(*get_direction)(struct gpio_chip *chip,
						unsigned offset);
	int			(*direction_input)(struct gpio_chip *chip,
						unsigned offset);
	int			(*direction_output)(struct gpio_chip *chip,
						unsigned offset, int value);
	int			(*get)(struct gpio_chip *chip,
						unsigned offset);
	void			(*set)(struct gpio_chip *chip,
						unsigned offset, int value);
<<<<<<< HEAD
	int			(*set_debounce)(struct gpio_chip *chip,
						unsigned offset,
						unsigned debounce);
=======
	void			(*set_multiple)(struct gpio_chip *chip,
						unsigned long *mask,
						unsigned long *bits);
	int			(*set_debounce)(struct gpio_chip *chip,
						unsigned offset,
						unsigned debounce);
	int			(*set_single_ended)(struct gpio_chip *chip,
						unsigned offset,
						enum single_ended_mode mode);
>>>>>>> v4.9.227

	int			(*to_irq)(struct gpio_chip *chip,
						unsigned offset);

	void			(*dbg_show)(struct seq_file *s,
						struct gpio_chip *chip);
	int			base;
	u16			ngpio;
<<<<<<< HEAD
	struct gpio_desc	*desc;
	const char		*const *names;
	bool			can_sleep;
	bool			irq_not_threaded;
	bool			exported;
=======
	const char		*const *names;
	bool			can_sleep;
	bool			irq_not_threaded;

#if IS_ENABLED(CONFIG_GPIO_GENERIC)
	unsigned long (*read_reg)(void __iomem *reg);
	void (*write_reg)(void __iomem *reg, unsigned long data);
	unsigned long (*pin2mask)(struct gpio_chip *gc, unsigned int pin);
	void __iomem *reg_dat;
	void __iomem *reg_set;
	void __iomem *reg_clr;
	void __iomem *reg_dir;
	int bgpio_bits;
	spinlock_t bgpio_lock;
	unsigned long bgpio_data;
	unsigned long bgpio_dir;
#endif
>>>>>>> v4.9.227

#ifdef CONFIG_GPIOLIB_IRQCHIP
	/*
	 * With CONFIG_GPIOLIB_IRQCHIP we get an irqchip inside the gpiolib
	 * to handle IRQs for most practical cases.
	 */
	struct irq_chip		*irqchip;
	struct irq_domain	*irqdomain;
	unsigned int		irq_base;
	irq_flow_handler_t	irq_handler;
	unsigned int		irq_default_type;
<<<<<<< HEAD
=======
	int			irq_parent;
	bool			irq_need_valid_mask;
	unsigned long		*irq_valid_mask;
	struct lock_class_key	*lock_key;
>>>>>>> v4.9.227
#endif

#if defined(CONFIG_OF_GPIO)
	/*
	 * If CONFIG_OF is enabled, then all GPIO controllers described in the
	 * device tree automatically may have an OF translation
	 */
	struct device_node *of_node;
	int of_gpio_n_cells;
	int (*of_xlate)(struct gpio_chip *gc,
			const struct of_phandle_args *gpiospec, u32 *flags);
#endif
<<<<<<< HEAD
#ifdef CONFIG_PINCTRL
	/*
	 * If CONFIG_PINCTRL is enabled, then gpio controllers can optionally
	 * describe the actual pin range which they serve in an SoC. This
	 * information would be used by pinctrl subsystem to configure
	 * corresponding pins for gpio usage.
	 */
	struct list_head pin_ranges;
#endif
=======
>>>>>>> v4.9.227
};

extern const char *gpiochip_is_requested(struct gpio_chip *chip,
			unsigned offset);

/* add/remove chips */
<<<<<<< HEAD
extern int gpiochip_add(struct gpio_chip *chip);
extern void gpiochip_remove(struct gpio_chip *chip);
=======
extern int gpiochip_add_data(struct gpio_chip *chip, void *data);
static inline int gpiochip_add(struct gpio_chip *chip)
{
	return gpiochip_add_data(chip, NULL);
}
extern void gpiochip_remove(struct gpio_chip *chip);
extern int devm_gpiochip_add_data(struct device *dev, struct gpio_chip *chip,
				  void *data);
extern void devm_gpiochip_remove(struct device *dev, struct gpio_chip *chip);

>>>>>>> v4.9.227
extern struct gpio_chip *gpiochip_find(void *data,
			      int (*match)(struct gpio_chip *chip, void *data));

/* lock/unlock as IRQ */
<<<<<<< HEAD
int gpio_lock_as_irq(struct gpio_chip *chip, unsigned int offset);
void gpio_unlock_as_irq(struct gpio_chip *chip, unsigned int offset);

struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc);

=======
int gpiochip_lock_as_irq(struct gpio_chip *chip, unsigned int offset);
void gpiochip_unlock_as_irq(struct gpio_chip *chip, unsigned int offset);
bool gpiochip_line_is_irq(struct gpio_chip *chip, unsigned int offset);

/* Line status inquiry for drivers */
bool gpiochip_line_is_open_drain(struct gpio_chip *chip, unsigned int offset);
bool gpiochip_line_is_open_source(struct gpio_chip *chip, unsigned int offset);

/* get driver data */
void *gpiochip_get_data(struct gpio_chip *chip);

struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc);

struct bgpio_pdata {
	const char *label;
	int base;
	int ngpio;
};

#if IS_ENABLED(CONFIG_GPIO_GENERIC)

int bgpio_init(struct gpio_chip *gc, struct device *dev,
	       unsigned long sz, void __iomem *dat, void __iomem *set,
	       void __iomem *clr, void __iomem *dirout, void __iomem *dirin,
	       unsigned long flags);

#define BGPIOF_BIG_ENDIAN		BIT(0)
#define BGPIOF_UNREADABLE_REG_SET	BIT(1) /* reg_set is unreadable */
#define BGPIOF_UNREADABLE_REG_DIR	BIT(2) /* reg_dir is unreadable */
#define BGPIOF_BIG_ENDIAN_BYTE_ORDER	BIT(3)
#define BGPIOF_READ_OUTPUT_REG_SET	BIT(4) /* reg_set stores output value */
#define BGPIOF_NO_OUTPUT		BIT(5) /* only input */

#endif

>>>>>>> v4.9.227
#ifdef CONFIG_GPIOLIB_IRQCHIP

void gpiochip_set_chained_irqchip(struct gpio_chip *gpiochip,
		struct irq_chip *irqchip,
		int parent_irq,
		irq_flow_handler_t parent_handler);

<<<<<<< HEAD
int gpiochip_irqchip_add(struct gpio_chip *gpiochip,
		struct irq_chip *irqchip,
		unsigned int first_irq,
		irq_flow_handler_t handler,
		unsigned int type);

#endif /* CONFIG_GPIOLIB_IRQCHIP */

=======
int _gpiochip_irqchip_add(struct gpio_chip *gpiochip,
			  struct irq_chip *irqchip,
			  unsigned int first_irq,
			  irq_flow_handler_t handler,
			  unsigned int type,
			  struct lock_class_key *lock_key);

#ifdef CONFIG_LOCKDEP
#define gpiochip_irqchip_add(...)				\
(								\
	({							\
		static struct lock_class_key _key;		\
		_gpiochip_irqchip_add(__VA_ARGS__, &_key);	\
	})							\
)
#else
#define gpiochip_irqchip_add(...)				\
	_gpiochip_irqchip_add(__VA_ARGS__, NULL)
#endif

#endif /* CONFIG_GPIOLIB_IRQCHIP */

int gpiochip_generic_request(struct gpio_chip *chip, unsigned offset);
void gpiochip_generic_free(struct gpio_chip *chip, unsigned offset);

#ifdef CONFIG_PINCTRL

/**
 * struct gpio_pin_range - pin range controlled by a gpio chip
 * @head: list for maintaining set of pin ranges, used internally
 * @pctldev: pinctrl device which handles corresponding pins
 * @range: actual range of pins controlled by a gpio controller
 */

struct gpio_pin_range {
	struct list_head node;
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range range;
};

int gpiochip_add_pin_range(struct gpio_chip *chip, const char *pinctl_name,
			   unsigned int gpio_offset, unsigned int pin_offset,
			   unsigned int npins);
int gpiochip_add_pingroup_range(struct gpio_chip *chip,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group);
void gpiochip_remove_pin_ranges(struct gpio_chip *chip);

#else

static inline int
gpiochip_add_pin_range(struct gpio_chip *chip, const char *pinctl_name,
		       unsigned int gpio_offset, unsigned int pin_offset,
		       unsigned int npins)
{
	return 0;
}
static inline int
gpiochip_add_pingroup_range(struct gpio_chip *chip,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group)
{
	return 0;
}

static inline void
gpiochip_remove_pin_ranges(struct gpio_chip *chip)
{
}

#endif /* CONFIG_PINCTRL */

>>>>>>> v4.9.227
struct gpio_desc *gpiochip_request_own_desc(struct gpio_chip *chip, u16 hwnum,
					    const char *label);
void gpiochip_free_own_desc(struct gpio_desc *desc);

#else /* CONFIG_GPIOLIB */

static inline struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc)
{
	/* GPIO can never have been requested */
	WARN_ON(1);
	return ERR_PTR(-ENODEV);
}

#endif /* CONFIG_GPIOLIB */

#endif
