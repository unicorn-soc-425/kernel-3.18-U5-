#ifndef TARGET_CORE_INTERNAL_H
#define TARGET_CORE_INTERNAL_H

<<<<<<< HEAD
=======
#define TARGET_CORE_NAME_MAX_LEN	64
#define TARGET_FABRIC_NAME_SIZE		32

struct target_backend {
	struct list_head list;

	const struct target_backend_ops *ops;

	struct config_item_type tb_dev_cit;
	struct config_item_type tb_dev_attrib_cit;
	struct config_item_type tb_dev_pr_cit;
	struct config_item_type tb_dev_wwn_cit;
	struct config_item_type tb_dev_alua_tg_pt_gps_cit;
	struct config_item_type tb_dev_stat_cit;
};

struct target_fabric_configfs {
	atomic_t		tf_access_cnt;
	struct list_head	tf_list;
	struct config_group	tf_group;
	struct config_group	tf_disc_group;
	const struct target_core_fabric_ops *tf_ops;

	struct config_item_type tf_discovery_cit;
	struct config_item_type	tf_wwn_cit;
	struct config_item_type tf_wwn_fabric_stats_cit;
	struct config_item_type tf_tpg_cit;
	struct config_item_type tf_tpg_base_cit;
	struct config_item_type tf_tpg_lun_cit;
	struct config_item_type tf_tpg_port_cit;
	struct config_item_type tf_tpg_port_stat_cit;
	struct config_item_type tf_tpg_np_cit;
	struct config_item_type tf_tpg_np_base_cit;
	struct config_item_type tf_tpg_attrib_cit;
	struct config_item_type tf_tpg_auth_cit;
	struct config_item_type tf_tpg_param_cit;
	struct config_item_type tf_tpg_nacl_cit;
	struct config_item_type tf_tpg_nacl_base_cit;
	struct config_item_type tf_tpg_nacl_attrib_cit;
	struct config_item_type tf_tpg_nacl_auth_cit;
	struct config_item_type tf_tpg_nacl_param_cit;
	struct config_item_type tf_tpg_nacl_stat_cit;
	struct config_item_type tf_tpg_mappedlun_cit;
	struct config_item_type tf_tpg_mappedlun_stat_cit;
};

>>>>>>> v4.9.227
/* target_core_alua.c */
extern struct t10_alua_lu_gp *default_lu_gp;

/* target_core_device.c */
<<<<<<< HEAD
struct se_dev_entry *core_get_se_deve_from_rtpi(struct se_node_acl *, u16);
int	core_free_device_list_for_node(struct se_node_acl *,
		struct se_portal_group *);
void	core_update_device_list_access(u32, u32, struct se_node_acl *);
int	core_enable_device_list_for_node(struct se_lun *, struct se_lun_acl *,
		u32, u32, struct se_node_acl *, struct se_portal_group *);
int	core_disable_device_list_for_node(struct se_lun *, struct se_lun_acl *,
		u32, u32, struct se_node_acl *, struct se_portal_group *);
void	core_clear_lun_from_tpg(struct se_lun *, struct se_portal_group *);
int	core_dev_export(struct se_device *, struct se_portal_group *,
		struct se_lun *);
void	core_dev_unexport(struct se_device *, struct se_portal_group *,
		struct se_lun *);
int	se_dev_set_task_timeout(struct se_device *, u32);
int	se_dev_set_max_unmap_lba_count(struct se_device *, u32);
int	se_dev_set_max_unmap_block_desc_count(struct se_device *, u32);
int	se_dev_set_unmap_granularity(struct se_device *, u32);
int	se_dev_set_unmap_granularity_alignment(struct se_device *, u32);
int	se_dev_set_max_write_same_len(struct se_device *, u32);
int	se_dev_set_emulate_model_alias(struct se_device *, int);
int	se_dev_set_emulate_dpo(struct se_device *, int);
int	se_dev_set_emulate_fua_write(struct se_device *, int);
int	se_dev_set_emulate_fua_read(struct se_device *, int);
int	se_dev_set_emulate_write_cache(struct se_device *, int);
int	se_dev_set_emulate_ua_intlck_ctrl(struct se_device *, int);
int	se_dev_set_emulate_tas(struct se_device *, int);
int	se_dev_set_emulate_tpu(struct se_device *, int);
int	se_dev_set_emulate_tpws(struct se_device *, int);
int	se_dev_set_emulate_caw(struct se_device *, int);
int	se_dev_set_emulate_3pc(struct se_device *, int);
int	se_dev_set_pi_prot_type(struct se_device *, int);
int	se_dev_set_pi_prot_format(struct se_device *, int);
int	se_dev_set_enforce_pr_isids(struct se_device *, int);
int	se_dev_set_force_pr_aptpl(struct se_device *, int);
int	se_dev_set_is_nonrot(struct se_device *, int);
int	se_dev_set_emulate_rest_reord(struct se_device *dev, int);
int	se_dev_set_queue_depth(struct se_device *, u32);
int	se_dev_set_max_sectors(struct se_device *, u32);
int	se_dev_set_fabric_max_sectors(struct se_device *, u32);
int	se_dev_set_optimal_sectors(struct se_device *, u32);
int	se_dev_set_block_size(struct se_device *, u32);
struct se_lun *core_dev_add_lun(struct se_portal_group *, struct se_device *, u32);
void	core_dev_del_lun(struct se_portal_group *, struct se_lun *);
struct se_lun *core_get_lun_from_tpg(struct se_portal_group *, u32);
struct se_lun_acl *core_dev_init_initiator_node_lun_acl(struct se_portal_group *,
		struct se_node_acl *, u32, int *);
int	core_dev_add_initiator_node_lun_acl(struct se_portal_group *,
		struct se_lun_acl *, u32, u32);
int	core_dev_del_initiator_node_lun_acl(struct se_portal_group *,
		struct se_lun *, struct se_lun_acl *);
=======
extern struct mutex g_device_mutex;
extern struct list_head g_device_list;

int	core_alloc_rtpi(struct se_lun *lun, struct se_device *dev);
struct se_dev_entry *core_get_se_deve_from_rtpi(struct se_node_acl *, u16);
void	target_pr_kref_release(struct kref *);
void	core_free_device_list_for_node(struct se_node_acl *,
		struct se_portal_group *);
void	core_update_device_list_access(u64, bool, struct se_node_acl *);
struct se_dev_entry *target_nacl_find_deve(struct se_node_acl *, u64);
int	core_enable_device_list_for_node(struct se_lun *, struct se_lun_acl *,
		u64, bool, struct se_node_acl *, struct se_portal_group *);
void	core_disable_device_list_for_node(struct se_lun *, struct se_dev_entry *,
		struct se_node_acl *, struct se_portal_group *);
void	core_clear_lun_from_tpg(struct se_lun *, struct se_portal_group *);
int	core_dev_add_lun(struct se_portal_group *, struct se_device *,
		struct se_lun *lun);
void	core_dev_del_lun(struct se_portal_group *, struct se_lun *);
struct se_lun_acl *core_dev_init_initiator_node_lun_acl(struct se_portal_group *,
		struct se_node_acl *, u64, int *);
int	core_dev_add_initiator_node_lun_acl(struct se_portal_group *,
		struct se_lun_acl *, struct se_lun *lun, bool);
int	core_dev_del_initiator_node_lun_acl(struct se_lun *,
		struct se_lun_acl *);
>>>>>>> v4.9.227
void	core_dev_free_initiator_node_lun_acl(struct se_portal_group *,
		struct se_lun_acl *lacl);
int	core_dev_setup_virtual_lun0(void);
void	core_dev_release_virtual_lun0(void);
struct se_device *target_alloc_device(struct se_hba *hba, const char *name);
int	target_configure_device(struct se_device *dev);
void	target_free_device(struct se_device *);

<<<<<<< HEAD
=======
/* target_core_configfs.c */
void	target_setup_backend_cits(struct target_backend *);

/* target_core_fabric_configfs.c */
int	target_fabric_setup_cits(struct target_fabric_configfs *);

/* target_core_fabric_lib.c */
int	target_get_pr_transport_id_len(struct se_node_acl *nacl,
		struct t10_pr_registration *pr_reg, int *format_code);
int	target_get_pr_transport_id(struct se_node_acl *nacl,
		struct t10_pr_registration *pr_reg, int *format_code,
		unsigned char *buf);
const char *target_parse_pr_out_transport_id(struct se_portal_group *tpg,
		const char *buf, u32 *out_tid_len, char **port_nexus_ptr);

>>>>>>> v4.9.227
/* target_core_hba.c */
struct se_hba *core_alloc_hba(const char *, u32, u32);
int	core_delete_hba(struct se_hba *);

/* target_core_tmr.c */
void	core_tmr_abort_task(struct se_device *, struct se_tmr_req *,
			struct se_session *);
int	core_tmr_lun_reset(struct se_device *, struct se_tmr_req *,
		struct list_head *, struct se_cmd *);

/* target_core_tpg.c */
extern struct se_device *g_lun0_dev;

struct se_node_acl *__core_tpg_get_initiator_node_acl(struct se_portal_group *tpg,
		const char *);
<<<<<<< HEAD
void	core_tpg_add_node_to_devs(struct se_node_acl *, struct se_portal_group *);
void	core_tpg_wait_for_nacl_pr_ref(struct se_node_acl *);
struct se_lun *core_tpg_alloc_lun(struct se_portal_group *, u32);
int	core_tpg_add_lun(struct se_portal_group *, struct se_lun *,
		u32, struct se_device *);
void core_tpg_remove_lun(struct se_portal_group *, struct se_lun *);
=======
void	core_tpg_add_node_to_devs(struct se_node_acl *, struct se_portal_group *,
				  struct se_lun *);
void	core_tpg_wait_for_nacl_pr_ref(struct se_node_acl *);
struct se_lun *core_tpg_alloc_lun(struct se_portal_group *, u64);
int	core_tpg_add_lun(struct se_portal_group *, struct se_lun *,
		bool, struct se_device *);
void core_tpg_remove_lun(struct se_portal_group *, struct se_lun *);
struct se_node_acl *core_tpg_add_initiator_node_acl(struct se_portal_group *tpg,
		const char *initiatorname);
void core_tpg_del_initiator_node_acl(struct se_node_acl *acl);
>>>>>>> v4.9.227

/* target_core_transport.c */
extern struct kmem_cache *se_tmr_req_cache;

int	init_se_kmem_caches(void);
void	release_se_kmem_caches(void);
u32	scsi_get_new_index(scsi_index_t);
void	transport_subsystem_check_init(void);
int	transport_cmd_finish_abort(struct se_cmd *, int);
unsigned char *transport_dump_cmd_direction(struct se_cmd *);
void	transport_dump_dev_state(struct se_device *, char *, int *);
void	transport_dump_dev_info(struct se_device *, struct se_lun *,
		unsigned long long, char *, int *);
void	transport_dump_vpd_proto_id(struct t10_vpd *, unsigned char *, int);
int	transport_dump_vpd_assoc(struct t10_vpd *, unsigned char *, int);
int	transport_dump_vpd_ident_type(struct t10_vpd *, unsigned char *, int);
int	transport_dump_vpd_ident(struct t10_vpd *, unsigned char *, int);
<<<<<<< HEAD
bool	target_stop_cmd(struct se_cmd *cmd, unsigned long *flags);
int	transport_clear_lun_ref(struct se_lun *);
void	transport_send_task_abort(struct se_cmd *);
sense_reason_t	target_cmd_size_check(struct se_cmd *cmd, unsigned int size);
void	target_qf_do_work(struct work_struct *work);
=======
void	transport_clear_lun_ref(struct se_lun *);
void	transport_send_task_abort(struct se_cmd *);
sense_reason_t	target_cmd_size_check(struct se_cmd *cmd, unsigned int size);
void	target_qf_do_work(struct work_struct *work);
bool	target_check_wce(struct se_device *dev);
bool	target_check_fua(struct se_device *dev);
void	__target_execute_cmd(struct se_cmd *, bool);
>>>>>>> v4.9.227

/* target_core_stat.c */
void	target_stat_setup_dev_default_groups(struct se_device *);
void	target_stat_setup_port_default_groups(struct se_lun *);
void	target_stat_setup_mappedlun_default_groups(struct se_lun_acl *);

<<<<<<< HEAD
=======
/* target_core_xcopy.c */
extern struct se_portal_group xcopy_pt_tpg;

/* target_core_configfs.c */
#define DB_ROOT_LEN		4096
#define	DB_ROOT_DEFAULT		"/var/target"

extern char db_root[];

>>>>>>> v4.9.227
#endif /* TARGET_CORE_INTERNAL_H */
