#include <linux/socket.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <rdma/ib_verbs.h>
#include <rdma/rdma_cm.h>
<<<<<<< HEAD

#define ISERT_RDMA_LISTEN_BACKLOG	10
#define ISCSI_ISER_SG_TABLESIZE		256
#define ISER_FASTREG_LI_WRID		0xffffffffffffffffULL
=======
#include <rdma/rw.h>
#include <scsi/iser.h>


#define DRV_NAME	"isert"
#define PFX		DRV_NAME ": "

#define isert_dbg(fmt, arg...)				 \
	do {						 \
		if (unlikely(isert_debug_level > 2))	 \
			printk(KERN_DEBUG PFX "%s: " fmt,\
				__func__ , ## arg);	 \
	} while (0)

#define isert_warn(fmt, arg...)				\
	do {						\
		if (unlikely(isert_debug_level > 0))	\
			pr_warn(PFX "%s: " fmt,         \
				__func__ , ## arg);	\
	} while (0)

#define isert_info(fmt, arg...)				\
	do {						\
		if (unlikely(isert_debug_level > 1))	\
			pr_info(PFX "%s: " fmt,         \
				__func__ , ## arg);	\
	} while (0)

#define isert_err(fmt, arg...) \
	pr_err(PFX "%s: " fmt, __func__ , ## arg)

/* Constant PDU lengths calculations */
#define ISER_HEADERS_LEN	(sizeof(struct iser_ctrl) + \
				 sizeof(struct iscsi_hdr))
#define ISER_RX_PAYLOAD_SIZE	(ISER_HEADERS_LEN + ISCSI_DEF_MAX_RECV_SEG_LEN)

/* QP settings */
/* Maximal bounds on received asynchronous PDUs */
#define ISERT_MAX_TX_MISC_PDUS	4 /* NOOP_IN(2) , ASYNC_EVENT(2)   */

#define ISERT_MAX_RX_MISC_PDUS	6 /*
				   * NOOP_OUT(2), TEXT(1),
				   * SCSI_TMFUNC(2), LOGOUT(1)
				   */

#define ISCSI_DEF_XMIT_CMDS_MAX 128 /* from libiscsi.h, must be power of 2 */

#define ISERT_QP_MAX_RECV_DTOS	(ISCSI_DEF_XMIT_CMDS_MAX)

#define ISERT_MIN_POSTED_RX	(ISCSI_DEF_XMIT_CMDS_MAX >> 2)

#define ISERT_QP_MAX_REQ_DTOS	(ISCSI_DEF_XMIT_CMDS_MAX +    \
				ISERT_MAX_TX_MISC_PDUS	+ \
				ISERT_MAX_RX_MISC_PDUS)

#define ISER_RX_PAD_SIZE	(ISCSI_DEF_MAX_RECV_SEG_LEN + 4096 - \
		(ISER_RX_PAYLOAD_SIZE + sizeof(u64) + sizeof(struct ib_sge) + \
		 sizeof(struct ib_cqe)))

#define ISCSI_ISER_SG_TABLESIZE		256
>>>>>>> v4.9.227

enum isert_desc_type {
	ISCSI_TX_CONTROL,
	ISCSI_TX_DATAIN
};

<<<<<<< HEAD
enum iser_ib_op_code {
	ISER_IB_RECV,
	ISER_IB_SEND,
	ISER_IB_RDMA_WRITE,
	ISER_IB_RDMA_READ,
};

enum iser_conn_state {
	ISER_CONN_INIT,
	ISER_CONN_UP,
=======
enum iser_conn_state {
	ISER_CONN_INIT,
	ISER_CONN_UP,
	ISER_CONN_BOUND,
>>>>>>> v4.9.227
	ISER_CONN_FULL_FEATURE,
	ISER_CONN_TERMINATING,
	ISER_CONN_DOWN,
};

struct iser_rx_desc {
<<<<<<< HEAD
	struct iser_hdr iser_header;
	struct iscsi_hdr iscsi_header;
	char		data[ISER_RECV_DATA_SEG_LEN];
	u64		dma_addr;
	struct ib_sge	rx_sg;
	char		pad[ISER_RX_PAD_SIZE];
} __packed;

struct iser_tx_desc {
	struct iser_hdr iser_header;
=======
	struct iser_ctrl iser_header;
	struct iscsi_hdr iscsi_header;
	char		data[ISCSI_DEF_MAX_RECV_SEG_LEN];
	u64		dma_addr;
	struct ib_sge	rx_sg;
	struct ib_cqe	rx_cqe;
	char		pad[ISER_RX_PAD_SIZE];
} __packed;

static inline struct iser_rx_desc *cqe_to_rx_desc(struct ib_cqe *cqe)
{
	return container_of(cqe, struct iser_rx_desc, rx_cqe);
}

struct iser_tx_desc {
	struct iser_ctrl iser_header;
>>>>>>> v4.9.227
	struct iscsi_hdr iscsi_header;
	enum isert_desc_type type;
	u64		dma_addr;
	struct ib_sge	tx_sg[2];
<<<<<<< HEAD
	int		num_sge;
	struct isert_cmd *isert_cmd;
	struct llist_node *comp_llnode_batch;
	struct llist_node comp_llnode;
	bool		llnode_active;
	struct ib_send_wr send_wr;
} __packed;

enum isert_indicator {
	ISERT_PROTECTED		= 1 << 0,
	ISERT_DATA_KEY_VALID	= 1 << 1,
	ISERT_PROT_KEY_VALID	= 1 << 2,
	ISERT_SIG_KEY_VALID	= 1 << 3,
};

struct pi_context {
	struct ib_mr		       *prot_mr;
	struct ib_fast_reg_page_list   *prot_frpl;
	struct ib_mr		       *sig_mr;
};

struct fast_reg_descriptor {
	struct list_head		list;
	struct ib_mr		       *data_mr;
	struct ib_fast_reg_page_list   *data_frpl;
	u8				ind;
	struct pi_context	       *pi_ctx;
};

struct isert_data_buf {
	struct scatterlist     *sg;
	int			nents;
	u32			sg_off;
	u32			len; /* cur_rdma_length */
	u32			offset;
	unsigned int		dma_nents;
	enum dma_data_direction dma_dir;
};

enum {
	DATA = 0,
	PROT = 1,
	SIG = 2,
};

struct isert_rdma_wr {
	struct list_head	wr_list;
	struct isert_cmd	*isert_cmd;
	enum iser_ib_op_code	iser_ib_op;
	struct ib_sge		*ib_sge;
	struct ib_sge		s_ib_sge;
	int			send_wr_num;
	struct ib_send_wr	*send_wr;
	struct ib_send_wr	s_send_wr;
	struct ib_sge		ib_sg[3];
	struct isert_data_buf	data;
	struct isert_data_buf	prot;
	struct fast_reg_descriptor *fr_desc;
};
=======
	struct ib_cqe	tx_cqe;
	int		num_sge;
	struct ib_send_wr send_wr;
} __packed;

static inline struct iser_tx_desc *cqe_to_tx_desc(struct ib_cqe *cqe)
{
	return container_of(cqe, struct iser_tx_desc, tx_cqe);
}
>>>>>>> v4.9.227

struct isert_cmd {
	uint32_t		read_stag;
	uint32_t		write_stag;
	uint64_t		read_va;
	uint64_t		write_va;
<<<<<<< HEAD
	u64			pdu_buf_dma;
	u32			pdu_buf_len;
	u32			read_va_off;
	u32			write_va_off;
	u32			rdma_wr_num;
	struct isert_conn	*conn;
	struct iscsi_cmd	*iscsi_cmd;
	struct iser_tx_desc	tx_desc;
	struct isert_rdma_wr	rdma_wr;
	struct work_struct	comp_work;
};

=======
	uint32_t		inv_rkey;
	u64			pdu_buf_dma;
	u32			pdu_buf_len;
	struct isert_conn	*conn;
	struct iscsi_cmd	*iscsi_cmd;
	struct iser_tx_desc	tx_desc;
	struct iser_rx_desc	*rx_desc;
	struct rdma_rw_ctx	rw;
	struct work_struct	comp_work;
	struct scatterlist	sg;
	bool			ctx_init_done;
};

static inline struct isert_cmd *tx_desc_to_cmd(struct iser_tx_desc *desc)
{
	return container_of(desc, struct isert_cmd, tx_desc);
}

>>>>>>> v4.9.227
struct isert_device;

struct isert_conn {
	enum iser_conn_state	state;
<<<<<<< HEAD
	int			post_recv_buf_count;
	atomic_t		post_send_buf_count;
	u32			responder_resources;
	u32			initiator_depth;
	bool			pi_support;
	u32			max_sge;
	char			*login_buf;
	char			*login_req_buf;
=======
	u32			responder_resources;
	u32			initiator_depth;
	bool			pi_support;
	struct iser_rx_desc	*login_req_buf;
>>>>>>> v4.9.227
	char			*login_rsp_buf;
	u64			login_req_dma;
	int			login_req_len;
	u64			login_rsp_dma;
<<<<<<< HEAD
	unsigned int		conn_rx_desc_head;
	struct iser_rx_desc	*conn_rx_descs;
	struct ib_recv_wr	conn_rx_wr[ISERT_MIN_POSTED_RX];
	struct iscsi_conn	*conn;
	struct list_head	conn_accept_node;
	struct completion	conn_login_comp;
	struct completion	login_req_comp;
	struct iser_tx_desc	conn_login_tx_desc;
	struct rdma_cm_id	*conn_cm_id;
	struct ib_pd		*conn_pd;
	struct ib_mr		*conn_mr;
	struct ib_qp		*conn_qp;
	struct isert_device	*conn_device;
	struct mutex		conn_mutex;
	struct completion	conn_wait;
	struct completion	conn_wait_comp_err;
	struct kref		conn_kref;
	struct list_head	conn_fr_pool;
	int			conn_fr_pool_size;
	/* lock to protect fastreg pool */
	spinlock_t		conn_lock;
	struct work_struct	release_work;
#define ISERT_COMP_BATCH_COUNT	8
	int			conn_comp_batch;
	struct llist_head	conn_comp_llist;
=======
	struct iser_rx_desc	*rx_descs;
	struct ib_recv_wr	rx_wr[ISERT_QP_MAX_RECV_DTOS];
	struct iscsi_conn	*conn;
	struct list_head	node;
	struct completion	login_comp;
	struct completion	login_req_comp;
	struct iser_tx_desc	login_tx_desc;
	struct rdma_cm_id	*cm_id;
	struct ib_qp		*qp;
	struct isert_device	*device;
	struct mutex		mutex;
	struct kref		kref;
	struct work_struct	release_work;
	bool                    logout_posted;
	bool                    snd_w_inv;
	wait_queue_head_t	rem_wait;
	bool			dev_removed;
>>>>>>> v4.9.227
};

#define ISERT_MAX_CQ 64

<<<<<<< HEAD
struct isert_cq_desc {
	struct isert_device	*device;
	int			cq_index;
	struct work_struct	cq_rx_work;
	struct work_struct	cq_tx_work;
};

struct isert_device {
	int			use_fastreg;
	bool			pi_capable;
	int			cqs_used;
	int			refcount;
	int			cq_active_qps[ISERT_MAX_CQ];
	struct ib_device	*ib_device;
	struct ib_cq		*dev_rx_cq[ISERT_MAX_CQ];
	struct ib_cq		*dev_tx_cq[ISERT_MAX_CQ];
	struct isert_cq_desc	*cq_desc;
	struct list_head	dev_node;
	struct ib_device_attr	dev_attr;
	int			(*reg_rdma_mem)(struct iscsi_conn *conn,
						    struct iscsi_cmd *cmd,
						    struct isert_rdma_wr *wr);
	void			(*unreg_rdma_mem)(struct isert_cmd *isert_cmd,
						  struct isert_conn *isert_conn);
=======
/**
 * struct isert_comp - iSER completion context
 *
 * @device:     pointer to device handle
 * @cq:         completion queue
 * @active_qps: Number of active QPs attached
 *              to completion context
 */
struct isert_comp {
	struct isert_device     *device;
	struct ib_cq		*cq;
	int                      active_qps;
};

struct isert_device {
	bool			pi_capable;
	int			refcount;
	struct ib_device	*ib_device;
	struct ib_pd		*pd;
	struct isert_comp	*comps;
	int                     comps_used;
	struct list_head	dev_node;
>>>>>>> v4.9.227
};

struct isert_np {
	struct iscsi_np         *np;
<<<<<<< HEAD
	struct semaphore	np_sem;
	struct rdma_cm_id	*np_cm_id;
	struct mutex		np_accept_mutex;
	struct list_head	np_accept_list;
	struct completion	np_login_comp;
=======
	struct semaphore	sem;
	struct rdma_cm_id	*cm_id;
	struct mutex		mutex;
	struct list_head	accepted;
	struct list_head	pending;
>>>>>>> v4.9.227
};
