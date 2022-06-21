/*
 * Renesas USB driver
 *
 * Copyright (C) 2011 Renesas Solutions Corp.
 * Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include <linux/delay.h>
#include <linux/slab.h>
#include "common.h"
#include "pipe.h"

/*
 *		macros
 */
#define usbhsp_addr_offset(p)	((usbhs_pipe_number(p) - 1) * 2)

#define usbhsp_flags_set(p, f)	((p)->flags |=  USBHS_PIPE_FLAGS_##f)
#define usbhsp_flags_clr(p, f)	((p)->flags &= ~USBHS_PIPE_FLAGS_##f)
#define usbhsp_flags_has(p, f)	((p)->flags &   USBHS_PIPE_FLAGS_##f)
#define usbhsp_flags_init(p)	do {(p)->flags = 0; } while (0)

/*
 * for debug
 */
static char *usbhsp_pipe_name[] = {
	[USB_ENDPOINT_XFER_CONTROL]	= "DCP",
	[USB_ENDPOINT_XFER_BULK]	= "BULK",
	[USB_ENDPOINT_XFER_INT]		= "INT",
	[USB_ENDPOINT_XFER_ISOC]	= "ISO",
};

char *usbhs_pipe_name(struct usbhs_pipe *pipe)
{
	return usbhsp_pipe_name[usbhs_pipe_type(pipe)];
}

<<<<<<< HEAD
=======
static struct renesas_usbhs_driver_pipe_config
*usbhsp_get_pipe_config(struct usbhs_priv *priv, int pipe_num)
{
	struct renesas_usbhs_driver_pipe_config *pipe_configs =
					usbhs_get_dparam(priv, pipe_configs);

	return &pipe_configs[pipe_num];
}

>>>>>>> v4.9.227
/*
 *		DCPCTR/PIPEnCTR functions
 */
static void usbhsp_pipectrl_set(struct usbhs_pipe *pipe, u16 mask, u16 val)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
	int offset = usbhsp_addr_offset(pipe);

	if (usbhs_pipe_is_dcp(pipe))
		usbhs_bset(priv, DCPCTR, mask, val);
	else
		usbhs_bset(priv, PIPEnCTR + offset, mask, val);
}

static u16 usbhsp_pipectrl_get(struct usbhs_pipe *pipe)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
	int offset = usbhsp_addr_offset(pipe);

	if (usbhs_pipe_is_dcp(pipe))
		return usbhs_read(priv, DCPCTR);
	else
		return usbhs_read(priv, PIPEnCTR + offset);
}

/*
 *		DCP/PIPE functions
 */
static void __usbhsp_pipe_xxx_set(struct usbhs_pipe *pipe,
				  u16 dcp_reg, u16 pipe_reg,
				  u16 mask, u16 val)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);

	if (usbhs_pipe_is_dcp(pipe))
		usbhs_bset(priv, dcp_reg, mask, val);
	else
		usbhs_bset(priv, pipe_reg, mask, val);
}

<<<<<<< HEAD
=======
static u16 __usbhsp_pipe_xxx_get(struct usbhs_pipe *pipe,
				 u16 dcp_reg, u16 pipe_reg)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);

	if (usbhs_pipe_is_dcp(pipe))
		return usbhs_read(priv, dcp_reg);
	else
		return usbhs_read(priv, pipe_reg);
}

>>>>>>> v4.9.227
/*
 *		DCPCFG/PIPECFG functions
 */
static void usbhsp_pipe_cfg_set(struct usbhs_pipe *pipe, u16 mask, u16 val)
{
	__usbhsp_pipe_xxx_set(pipe, DCPCFG, PIPECFG, mask, val);
}

<<<<<<< HEAD
=======
static u16 usbhsp_pipe_cfg_get(struct usbhs_pipe *pipe)
{
	return __usbhsp_pipe_xxx_get(pipe, DCPCFG, PIPECFG);
}

>>>>>>> v4.9.227
/*
 *		PIPEnTRN/PIPEnTRE functions
 */
static void usbhsp_pipe_trn_set(struct usbhs_pipe *pipe, u16 mask, u16 val)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
	struct device *dev = usbhs_priv_to_dev(priv);
	int num = usbhs_pipe_number(pipe);
	u16 reg;

	/*
	 * It is impossible to calculate address,
	 * since PIPEnTRN addresses were mapped randomly.
	 */
#define CASE_PIPExTRN(a)		\
	case 0x ## a:			\
		reg = PIPE ## a ## TRN;	\
		break;

	switch (num) {
	CASE_PIPExTRN(1);
	CASE_PIPExTRN(2);
	CASE_PIPExTRN(3);
	CASE_PIPExTRN(4);
	CASE_PIPExTRN(5);
	CASE_PIPExTRN(B);
	CASE_PIPExTRN(C);
	CASE_PIPExTRN(D);
	CASE_PIPExTRN(E);
	CASE_PIPExTRN(F);
	CASE_PIPExTRN(9);
	CASE_PIPExTRN(A);
	default:
		dev_err(dev, "unknown pipe (%d)\n", num);
		return;
	}
	__usbhsp_pipe_xxx_set(pipe, 0, reg, mask, val);
}

static void usbhsp_pipe_tre_set(struct usbhs_pipe *pipe, u16 mask, u16 val)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
	struct device *dev = usbhs_priv_to_dev(priv);
	int num = usbhs_pipe_number(pipe);
	u16 reg;

	/*
	 * It is impossible to calculate address,
	 * since PIPEnTRE addresses were mapped randomly.
	 */
#define CASE_PIPExTRE(a)			\
	case 0x ## a:				\
		reg = PIPE ## a ## TRE;		\
		break;

	switch (num) {
	CASE_PIPExTRE(1);
	CASE_PIPExTRE(2);
	CASE_PIPExTRE(3);
	CASE_PIPExTRE(4);
	CASE_PIPExTRE(5);
	CASE_PIPExTRE(B);
	CASE_PIPExTRE(C);
	CASE_PIPExTRE(D);
	CASE_PIPExTRE(E);
	CASE_PIPExTRE(F);
	CASE_PIPExTRE(9);
	CASE_PIPExTRE(A);
	default:
		dev_err(dev, "unknown pipe (%d)\n", num);
		return;
	}

	__usbhsp_pipe_xxx_set(pipe, 0, reg, mask, val);
}

/*
 *		PIPEBUF
 */
static void usbhsp_pipe_buf_set(struct usbhs_pipe *pipe, u16 mask, u16 val)
{
	if (usbhs_pipe_is_dcp(pipe))
		return;

	__usbhsp_pipe_xxx_set(pipe, 0, PIPEBUF, mask, val);
}

/*
 *		DCPMAXP/PIPEMAXP
 */
static void usbhsp_pipe_maxp_set(struct usbhs_pipe *pipe, u16 mask, u16 val)
{
	__usbhsp_pipe_xxx_set(pipe, DCPMAXP, PIPEMAXP, mask, val);
}

/*
 *		pipe control functions
 */
static void usbhsp_pipe_select(struct usbhs_pipe *pipe)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);

	/*
	 * On pipe, this is necessary before
	 * accesses to below registers.
	 *
	 * PIPESEL	: usbhsp_pipe_select
	 * PIPECFG	: usbhsp_pipe_cfg_xxx
	 * PIPEBUF	: usbhsp_pipe_buf_xxx
	 * PIPEMAXP	: usbhsp_pipe_maxp_xxx
	 * PIPEPERI
	 */

	/*
	 * if pipe is dcp, no pipe is selected.
	 * it is no problem, because dcp have its register
	 */
	usbhs_write(priv, PIPESEL, 0xF & usbhs_pipe_number(pipe));
}

static int usbhsp_pipe_barrier(struct usbhs_pipe *pipe)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
	int timeout = 1024;
<<<<<<< HEAD
	u16 val;
=======
	u16 mask = usbhs_mod_is_host(priv) ? (CSSTS | PID_MASK) : PID_MASK;
>>>>>>> v4.9.227

	/*
	 * make sure....
	 *
	 * Modify these bits when CSSTS = 0, PID = NAK, and no pipe number is
	 * specified by the CURPIPE bits.
	 * When changing the setting of this bit after changing
	 * the PID bits for the selected pipe from BUF to NAK,
	 * check that CSSTS = 0 and PBUSY = 0.
	 */

	/*
	 * CURPIPE bit = 0
	 *
	 * see also
	 *  "Operation"
	 *  - "Pipe Control"
	 *   - "Pipe Control Registers Switching Procedure"
	 */
	usbhs_write(priv, CFIFOSEL, 0);
	usbhs_pipe_disable(pipe);

	do {
<<<<<<< HEAD
		val  = usbhsp_pipectrl_get(pipe);
		val &= CSSTS | PID_MASK;
		if (!val)
=======
		if (!(usbhsp_pipectrl_get(pipe) & mask))
>>>>>>> v4.9.227
			return 0;

		udelay(10);

	} while (timeout--);

	return -EBUSY;
}

int usbhs_pipe_is_accessible(struct usbhs_pipe *pipe)
{
	u16 val;

	val = usbhsp_pipectrl_get(pipe);
	if (val & BSTS)
		return 0;

	return -EBUSY;
}

<<<<<<< HEAD
=======
bool usbhs_pipe_contains_transmittable_data(struct usbhs_pipe *pipe)
{
	u16 val;

	/* Do not support for DCP pipe */
	if (usbhs_pipe_is_dcp(pipe))
		return false;

	val = usbhsp_pipectrl_get(pipe);
	if (val & INBUFM)
		return true;

	return false;
}

>>>>>>> v4.9.227
/*
 *		PID ctrl
 */
static void __usbhsp_pid_try_nak_if_stall(struct usbhs_pipe *pipe)
{
	u16 pid = usbhsp_pipectrl_get(pipe);

	pid &= PID_MASK;

	/*
	 * see
	 * "Pipe n Control Register" - "PID"
	 */
	switch (pid) {
	case PID_STALL11:
		usbhsp_pipectrl_set(pipe, PID_MASK, PID_STALL10);
		/* fall-through */
	case PID_STALL10:
		usbhsp_pipectrl_set(pipe, PID_MASK, PID_NAK);
	}
}

void usbhs_pipe_disable(struct usbhs_pipe *pipe)
{
	int timeout = 1024;
	u16 val;

	/* see "Pipe n Control Register" - "PID" */
	__usbhsp_pid_try_nak_if_stall(pipe);

	usbhsp_pipectrl_set(pipe, PID_MASK, PID_NAK);

	do {
		val  = usbhsp_pipectrl_get(pipe);
		val &= PBUSY;
		if (!val)
			break;

		udelay(10);
	} while (timeout--);
}

void usbhs_pipe_enable(struct usbhs_pipe *pipe)
{
	/* see "Pipe n Control Register" - "PID" */
	__usbhsp_pid_try_nak_if_stall(pipe);

	usbhsp_pipectrl_set(pipe, PID_MASK, PID_BUF);
}

void usbhs_pipe_stall(struct usbhs_pipe *pipe)
{
	u16 pid = usbhsp_pipectrl_get(pipe);

	pid &= PID_MASK;

	/*
	 * see
	 * "Pipe n Control Register" - "PID"
	 */
	switch (pid) {
	case PID_NAK:
		usbhsp_pipectrl_set(pipe, PID_MASK, PID_STALL10);
		break;
	case PID_BUF:
		usbhsp_pipectrl_set(pipe, PID_MASK, PID_STALL11);
		break;
	}
}

int usbhs_pipe_is_stall(struct usbhs_pipe *pipe)
{
	u16 pid = usbhsp_pipectrl_get(pipe) & PID_MASK;

	return (int)(pid == PID_STALL10 || pid == PID_STALL11);
}

void usbhs_pipe_set_trans_count_if_bulk(struct usbhs_pipe *pipe, int len)
{
	if (!usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_BULK))
		return;

	/*
	 * clear and disable transfer counter for IN/OUT pipe
	 */
	usbhsp_pipe_tre_set(pipe, TRCLR | TRENB, TRCLR);

	/*
	 * Only IN direction bulk pipe can use transfer count.
	 * Without using this function,
	 * received data will break if it was large data size.
	 * see PIPEnTRN/PIPEnTRE for detail
	 */
	if (usbhs_pipe_is_dir_in(pipe)) {
		int maxp = usbhs_pipe_get_maxpacket(pipe);

		usbhsp_pipe_trn_set(pipe, 0xffff, DIV_ROUND_UP(len, maxp));
		usbhsp_pipe_tre_set(pipe, TRENB, TRENB); /* enable */
	}
}


/*
 *		pipe setup
 */
<<<<<<< HEAD
static int usbhsp_possible_double_buffer(struct usbhs_pipe *pipe)
{
	/*
	 * only ISO / BULK pipe can use double buffer
	 */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_BULK) ||
	    usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_ISOC))
		return 1;

	return 0;
}

static u16 usbhsp_setup_pipecfg(struct usbhs_pipe *pipe,
				int is_host,
				int dir_in)
=======
static int usbhsp_setup_pipecfg(struct usbhs_pipe *pipe, int is_host,
				int dir_in, u16 *pipecfg)
>>>>>>> v4.9.227
{
	u16 type = 0;
	u16 bfre = 0;
	u16 dblb = 0;
	u16 cntmd = 0;
	u16 dir = 0;
	u16 epnum = 0;
	u16 shtnak = 0;
	u16 type_array[] = {
		[USB_ENDPOINT_XFER_BULK] = TYPE_BULK,
		[USB_ENDPOINT_XFER_INT]  = TYPE_INT,
		[USB_ENDPOINT_XFER_ISOC] = TYPE_ISO,
	};
<<<<<<< HEAD
	int is_double = usbhsp_possible_double_buffer(pipe);
=======
>>>>>>> v4.9.227

	if (usbhs_pipe_is_dcp(pipe))
		return -EINVAL;

	/*
	 * PIPECFG
	 *
	 * see
	 *  - "Register Descriptions" - "PIPECFG" register
	 *  - "Features"  - "Pipe configuration"
	 *  - "Operation" - "Pipe Control"
	 */

	/* TYPE */
	type = type_array[usbhs_pipe_type(pipe)];

	/* BFRE */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_ISOC) ||
	    usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_BULK))
		bfre = 0; /* FIXME */

<<<<<<< HEAD
	/* DBLB */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_ISOC) ||
	    usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_BULK))
		dblb = (is_double) ? DBLB : 0;
=======
	/* DBLB: see usbhs_pipe_config_update() */
>>>>>>> v4.9.227

	/* CNTMD */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_BULK))
		cntmd = 0; /* FIXME */

	/* DIR */
	if (dir_in)
		usbhsp_flags_set(pipe, IS_DIR_HOST);

	if (!!is_host ^ !!dir_in)
		dir |= DIR_OUT;

	if (!dir)
		usbhsp_flags_set(pipe, IS_DIR_IN);

	/* SHTNAK */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_BULK) &&
	    !dir)
		shtnak = SHTNAK;

	/* EPNUM */
	epnum = 0; /* see usbhs_pipe_config_update() */
<<<<<<< HEAD

	return	type	|
		bfre	|
		dblb	|
		cntmd	|
		dir	|
		shtnak	|
		epnum;
=======
	*pipecfg = type		|
		   bfre		|
		   dblb		|
		   cntmd	|
		   dir		|
		   shtnak	|
		   epnum;
	return 0;
>>>>>>> v4.9.227
}

static u16 usbhsp_setup_pipebuff(struct usbhs_pipe *pipe)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
<<<<<<< HEAD
	struct usbhs_pipe_info *info = usbhs_priv_to_pipeinfo(priv);
	struct device *dev = usbhs_priv_to_dev(priv);
	int pipe_num = usbhs_pipe_number(pipe);
	int is_double = usbhsp_possible_double_buffer(pipe);
	u16 buff_size;
	u16 bufnmb;
	u16 bufnmb_cnt;
=======
	struct device *dev = usbhs_priv_to_dev(priv);
	int pipe_num = usbhs_pipe_number(pipe);
	u16 buff_size;
	u16 bufnmb;
	u16 bufnmb_cnt;
	struct renesas_usbhs_driver_pipe_config *pipe_config =
					usbhsp_get_pipe_config(priv, pipe_num);
>>>>>>> v4.9.227

	/*
	 * PIPEBUF
	 *
	 * see
	 *  - "Register Descriptions" - "PIPEBUF" register
	 *  - "Features"  - "Pipe configuration"
	 *  - "Operation" - "FIFO Buffer Memory"
	 *  - "Operation" - "Pipe Control"
<<<<<<< HEAD
	 *
	 * ex) if pipe6 - pipe9 are USB_ENDPOINT_XFER_INT (SH7724)
	 *
	 * BUFNMB:	PIPE
	 * 0:		pipe0 (DCP 256byte)
	 * 1:		-
	 * 2:		-
	 * 3:		-
	 * 4:		pipe6 (INT 64byte)
	 * 5:		pipe7 (INT 64byte)
	 * 6:		pipe8 (INT 64byte)
	 * 7:		pipe9 (INT 64byte)
	 * 8 - xx:	free (for BULK, ISOC)
	 */

	/*
	 * FIXME
	 *
	 * it doesn't have good buffer allocator
	 *
	 * DCP : 256 byte
	 * BULK: 512 byte
	 * INT :  64 byte
	 * ISOC: 512 byte
	 */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_CONTROL))
		buff_size = 256;
	else if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_INT))
		buff_size = 64;
	else
		buff_size = 512;
=======
	 */
	buff_size = pipe_config->bufsize;
	bufnmb = pipe_config->bufnum;
>>>>>>> v4.9.227

	/* change buff_size to register value */
	bufnmb_cnt = (buff_size / 64) - 1;

<<<<<<< HEAD
	/* BUFNMB has been reserved for INT pipe
	 * see above */
	if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_INT)) {
		bufnmb = pipe_num - 2;
	} else {
		bufnmb = info->bufnmb_last;
		info->bufnmb_last += bufnmb_cnt + 1;

		/*
		 * double buffer
		 */
		if (is_double)
			info->bufnmb_last += bufnmb_cnt + 1;
	}

=======
>>>>>>> v4.9.227
	dev_dbg(dev, "pipe : %d : buff_size 0x%x: bufnmb 0x%x\n",
		pipe_num, buff_size, bufnmb);

	return	(0x1f & bufnmb_cnt)	<< 10 |
		(0xff & bufnmb)		<<  0;
}

void usbhs_pipe_config_update(struct usbhs_pipe *pipe, u16 devsel,
			      u16 epnum, u16 maxp)
{
<<<<<<< HEAD
	if (devsel > 0xA) {
		struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
=======
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);
	int pipe_num = usbhs_pipe_number(pipe);
	struct renesas_usbhs_driver_pipe_config *pipe_config =
					usbhsp_get_pipe_config(priv, pipe_num);
	u16 dblb = pipe_config->double_buf ? DBLB : 0;

	if (devsel > 0xA) {
>>>>>>> v4.9.227
		struct device *dev = usbhs_priv_to_dev(priv);

		dev_err(dev, "devsel error %d\n", devsel);

		devsel = 0;
	}

	usbhsp_pipe_barrier(pipe);

	pipe->maxp = maxp;

	usbhsp_pipe_select(pipe);
	usbhsp_pipe_maxp_set(pipe, 0xFFFF,
			     (devsel << 12) |
			     maxp);

	if (!usbhs_pipe_is_dcp(pipe))
<<<<<<< HEAD
		usbhsp_pipe_cfg_set(pipe,  0x000F, epnum);
=======
		usbhsp_pipe_cfg_set(pipe,  0x000F | DBLB, epnum | dblb);
>>>>>>> v4.9.227
}

/*
 *		pipe control
 */
int usbhs_pipe_get_maxpacket(struct usbhs_pipe *pipe)
{
	/*
	 * see
	 *	usbhs_pipe_config_update()
	 *	usbhs_dcp_malloc()
	 */
	return pipe->maxp;
}

int usbhs_pipe_is_dir_in(struct usbhs_pipe *pipe)
{
	return usbhsp_flags_has(pipe, IS_DIR_IN);
}

int usbhs_pipe_is_dir_host(struct usbhs_pipe *pipe)
{
	return usbhsp_flags_has(pipe, IS_DIR_HOST);
}

int usbhs_pipe_is_running(struct usbhs_pipe *pipe)
{
	return usbhsp_flags_has(pipe, IS_RUNNING);
}

void usbhs_pipe_running(struct usbhs_pipe *pipe, int running)
{
	if (running)
		usbhsp_flags_set(pipe, IS_RUNNING);
	else
		usbhsp_flags_clr(pipe, IS_RUNNING);
}

void usbhs_pipe_data_sequence(struct usbhs_pipe *pipe, int sequence)
{
	u16 mask = (SQCLR | SQSET);
	u16 val;

	/*
	 * sequence
	 *  0  : data0
	 *  1  : data1
	 *  -1 : no change
	 */
	switch (sequence) {
	case 0:
		val = SQCLR;
		break;
	case 1:
		val = SQSET;
		break;
	default:
		return;
	}

	usbhsp_pipectrl_set(pipe, mask, val);
}

<<<<<<< HEAD
void usbhs_pipe_clear(struct usbhs_pipe *pipe)
{
	usbhsp_pipectrl_set(pipe, ACLRM, ACLRM);
	usbhsp_pipectrl_set(pipe, ACLRM, 0);
=======
static int usbhs_pipe_get_data_sequence(struct usbhs_pipe *pipe)
{
	return !!(usbhsp_pipectrl_get(pipe) & SQMON);
}

void usbhs_pipe_clear(struct usbhs_pipe *pipe)
{
	if (usbhs_pipe_is_dcp(pipe)) {
		usbhs_fifo_clear_dcp(pipe);
	} else {
		usbhsp_pipectrl_set(pipe, ACLRM, ACLRM);
		usbhsp_pipectrl_set(pipe, ACLRM, 0);
	}
}

void usbhs_pipe_config_change_bfre(struct usbhs_pipe *pipe, int enable)
{
	int sequence;

	if (usbhs_pipe_is_dcp(pipe))
		return;

	usbhsp_pipe_select(pipe);
	/* check if the driver needs to change the BFRE value */
	if (!(enable ^ !!(usbhsp_pipe_cfg_get(pipe) & BFRE)))
		return;

	sequence = usbhs_pipe_get_data_sequence(pipe);
	usbhsp_pipe_cfg_set(pipe, BFRE, enable ? BFRE : 0);
	usbhs_pipe_clear(pipe);
	usbhs_pipe_data_sequence(pipe, sequence);
>>>>>>> v4.9.227
}

static struct usbhs_pipe *usbhsp_get_pipe(struct usbhs_priv *priv, u32 type)
{
	struct usbhs_pipe *pos, *pipe;
	int i;

	/*
	 * find target pipe
	 */
	pipe = NULL;
	usbhs_for_each_pipe_with_dcp(pos, priv, i) {
		if (!usbhs_pipe_type_is(pos, type))
			continue;
		if (usbhsp_flags_has(pos, IS_USED))
			continue;

		pipe = pos;
		break;
	}

	if (!pipe)
		return NULL;

	/*
	 * initialize pipe flags
	 */
	usbhsp_flags_init(pipe);
	usbhsp_flags_set(pipe, IS_USED);

	return pipe;
}

static void usbhsp_put_pipe(struct usbhs_pipe *pipe)
{
	usbhsp_flags_init(pipe);
}

void usbhs_pipe_init(struct usbhs_priv *priv,
<<<<<<< HEAD
		     int (*dma_map_ctrl)(struct usbhs_pkt *pkt, int map))
=======
		     int (*dma_map_ctrl)(struct device *dma_dev,
					 struct usbhs_pkt *pkt, int map))
>>>>>>> v4.9.227
{
	struct usbhs_pipe_info *info = usbhs_priv_to_pipeinfo(priv);
	struct usbhs_pipe *pipe;
	int i;

<<<<<<< HEAD
	/*
	 * FIXME
	 *
	 * driver needs good allocator.
	 *
	 * find first free buffer area (BULK, ISOC)
	 * (DCP, INT area is fixed)
	 *
	 * buffer number 0 - 3 have been reserved for DCP
	 * see
	 *	usbhsp_to_bufnmb
	 */
	info->bufnmb_last = 4;
	usbhs_for_each_pipe_with_dcp(pipe, priv, i) {
		if (usbhs_pipe_type_is(pipe, USB_ENDPOINT_XFER_INT))
			info->bufnmb_last++;

=======
	usbhs_for_each_pipe_with_dcp(pipe, priv, i) {
>>>>>>> v4.9.227
		usbhsp_flags_init(pipe);
		pipe->fifo = NULL;
		pipe->mod_private = NULL;
		INIT_LIST_HEAD(&pipe->list);

		/* pipe force init */
		usbhs_pipe_clear(pipe);
	}

	info->dma_map_ctrl = dma_map_ctrl;
}

struct usbhs_pipe *usbhs_pipe_malloc(struct usbhs_priv *priv,
				     int endpoint_type,
				     int dir_in)
{
	struct device *dev = usbhs_priv_to_dev(priv);
	struct usbhs_pipe *pipe;
	int is_host = usbhs_mod_is_host(priv);
	int ret;
	u16 pipecfg, pipebuf;

	pipe = usbhsp_get_pipe(priv, endpoint_type);
	if (!pipe) {
		dev_err(dev, "can't get pipe (%s)\n",
			usbhsp_pipe_name[endpoint_type]);
		return NULL;
	}

	INIT_LIST_HEAD(&pipe->list);

	usbhs_pipe_disable(pipe);

	/* make sure pipe is not busy */
	ret = usbhsp_pipe_barrier(pipe);
	if (ret < 0) {
		dev_err(dev, "pipe setup failed %d\n", usbhs_pipe_number(pipe));
		return NULL;
	}

<<<<<<< HEAD
	pipecfg  = usbhsp_setup_pipecfg(pipe, is_host, dir_in);
=======
	if (usbhsp_setup_pipecfg(pipe, is_host, dir_in, &pipecfg)) {
		dev_err(dev, "can't setup pipe\n");
		return NULL;
	}

>>>>>>> v4.9.227
	pipebuf  = usbhsp_setup_pipebuff(pipe);

	usbhsp_pipe_select(pipe);
	usbhsp_pipe_cfg_set(pipe, 0xFFFF, pipecfg);
	usbhsp_pipe_buf_set(pipe, 0xFFFF, pipebuf);
	usbhs_pipe_clear(pipe);

	usbhs_pipe_sequence_data0(pipe);

	dev_dbg(dev, "enable pipe %d : %s (%s)\n",
		usbhs_pipe_number(pipe),
		usbhs_pipe_name(pipe),
		usbhs_pipe_is_dir_in(pipe) ? "in" : "out");

	/*
	 * epnum / maxp are still not set to this pipe.
	 * call usbhs_pipe_config_update() after this function !!
	 */

	return pipe;
}

void usbhs_pipe_free(struct usbhs_pipe *pipe)
{
	usbhsp_put_pipe(pipe);
}

void usbhs_pipe_select_fifo(struct usbhs_pipe *pipe, struct usbhs_fifo *fifo)
{
	if (pipe->fifo)
		pipe->fifo->pipe = NULL;

	pipe->fifo = fifo;

	if (fifo)
		fifo->pipe = pipe;
}


/*
 *		dcp control
 */
struct usbhs_pipe *usbhs_dcp_malloc(struct usbhs_priv *priv)
{
	struct usbhs_pipe *pipe;

	pipe = usbhsp_get_pipe(priv, USB_ENDPOINT_XFER_CONTROL);
	if (!pipe)
		return NULL;

	INIT_LIST_HEAD(&pipe->list);

	/*
	 * call usbhs_pipe_config_update() after this function !!
	 */

	return pipe;
}

void usbhs_dcp_control_transfer_done(struct usbhs_pipe *pipe)
{
	struct usbhs_priv *priv = usbhs_pipe_to_priv(pipe);

	WARN_ON(!usbhs_pipe_is_dcp(pipe));

	usbhs_pipe_enable(pipe);

	if (!usbhs_mod_is_host(priv)) /* funconly */
		usbhsp_pipectrl_set(pipe, CCPL, CCPL);
}

void usbhs_dcp_dir_for_host(struct usbhs_pipe *pipe, int dir_out)
{
	usbhsp_pipe_cfg_set(pipe, DIR_OUT,
			    dir_out ? DIR_OUT : 0);
}

/*
 *		pipe module function
 */
int usbhs_pipe_probe(struct usbhs_priv *priv)
{
	struct usbhs_pipe_info *info = usbhs_priv_to_pipeinfo(priv);
	struct usbhs_pipe *pipe;
	struct device *dev = usbhs_priv_to_dev(priv);
<<<<<<< HEAD
	u32 *pipe_type = usbhs_get_dparam(priv, pipe_type);
=======
	struct renesas_usbhs_driver_pipe_config *pipe_configs =
					usbhs_get_dparam(priv, pipe_configs);
>>>>>>> v4.9.227
	int pipe_size = usbhs_get_dparam(priv, pipe_size);
	int i;

	/* This driver expects 1st pipe is DCP */
<<<<<<< HEAD
	if (pipe_type[0] != USB_ENDPOINT_XFER_CONTROL) {
=======
	if (pipe_configs[0].type != USB_ENDPOINT_XFER_CONTROL) {
>>>>>>> v4.9.227
		dev_err(dev, "1st PIPE is not DCP\n");
		return -EINVAL;
	}

	info->pipe = kzalloc(sizeof(struct usbhs_pipe) * pipe_size, GFP_KERNEL);
<<<<<<< HEAD
	if (!info->pipe) {
		dev_err(dev, "Could not allocate pipe\n");
		return -ENOMEM;
	}
=======
	if (!info->pipe)
		return -ENOMEM;
>>>>>>> v4.9.227

	info->size = pipe_size;

	/*
	 * init pipe
	 */
	usbhs_for_each_pipe_with_dcp(pipe, priv, i) {
		pipe->priv = priv;

		usbhs_pipe_type(pipe) =
<<<<<<< HEAD
			pipe_type[i] & USB_ENDPOINT_XFERTYPE_MASK;

		dev_dbg(dev, "pipe %x\t: %s\n",
			i, usbhsp_pipe_name[pipe_type[i]]);
=======
			pipe_configs[i].type & USB_ENDPOINT_XFERTYPE_MASK;

		dev_dbg(dev, "pipe %x\t: %s\n",
			i, usbhsp_pipe_name[pipe_configs[i].type]);
>>>>>>> v4.9.227
	}

	return 0;
}

void usbhs_pipe_remove(struct usbhs_priv *priv)
{
	struct usbhs_pipe_info *info = usbhs_priv_to_pipeinfo(priv);

	kfree(info->pipe);
}
