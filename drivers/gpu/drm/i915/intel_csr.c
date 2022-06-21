/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */
#include <linux/firmware.h>
#include "i915_drv.h"
#include "i915_reg.h"

/**
 * DOC: csr support for dmc
 *
 * Display Context Save and Restore (CSR) firmware support added from gen9
 * onwards to drive newly added DMC (Display microcontroller) in display
 * engine to save and restore the state of display engine when it enter into
 * low-power state and comes back to normal.
<<<<<<< HEAD
 *
 * Firmware loading status will be one of the below states: FW_UNINITIALIZED,
 * FW_LOADED, FW_FAILED.
 *
 * Once the firmware is written into the registers status will be moved from
 * FW_UNINITIALIZED to FW_LOADED and for any erroneous condition status will
 * be moved to FW_FAILED.
 */

#define I915_CSR_SKL "i915/skl_dmc_ver1.bin"
#define I915_CSR_BXT "i915/bxt_dmc_ver1.bin"

MODULE_FIRMWARE(I915_CSR_SKL);
MODULE_FIRMWARE(I915_CSR_BXT);

/*
* SKL CSR registers for DC5 and DC6
*/
#define CSR_PROGRAM(i)			(0x80000 + (i) * 4)
#define CSR_SSP_BASE_ADDR_GEN9		0x00002FC0
#define CSR_HTP_ADDR_SKL		0x00500034
#define CSR_SSP_BASE			0x8F074
#define CSR_HTP_SKL			0x8F004
#define CSR_LAST_WRITE			0x8F034
#define CSR_LAST_WRITE_VALUE		0xc003b400
/* MMIO address range for CSR program (0x80000 - 0x82FFF) */
#define CSR_MAX_FW_SIZE			0x2FFF
#define CSR_DEFAULT_FW_OFFSET		0xFFFFFFFF
#define CSR_MMIO_START_RANGE	0x80000
#define CSR_MMIO_END_RANGE		0x8FFFF
=======
 */

#define I915_CSR_KBL "i915/kbl_dmc_ver1_01.bin"
MODULE_FIRMWARE(I915_CSR_KBL);
#define KBL_CSR_VERSION_REQUIRED	CSR_VERSION(1, 1)

#define I915_CSR_SKL "i915/skl_dmc_ver1_26.bin"
MODULE_FIRMWARE(I915_CSR_SKL);
#define SKL_CSR_VERSION_REQUIRED	CSR_VERSION(1, 26)

#define I915_CSR_BXT "i915/bxt_dmc_ver1_07.bin"
MODULE_FIRMWARE(I915_CSR_BXT);
#define BXT_CSR_VERSION_REQUIRED	CSR_VERSION(1, 7)

#define FIRMWARE_URL  "https://01.org/linuxgraphics/intel-linux-graphics-firmwares"




#define CSR_MAX_FW_SIZE			0x2FFF
#define CSR_DEFAULT_FW_OFFSET		0xFFFFFFFF
>>>>>>> v4.9.227

struct intel_css_header {
	/* 0x09 for DMC */
	uint32_t module_type;

	/* Includes the DMC specific header in dwords */
	uint32_t header_len;

	/* always value would be 0x10000 */
	uint32_t header_ver;

	/* Not used */
	uint32_t module_id;

	/* Not used */
	uint32_t module_vendor;

	/* in YYYYMMDD format */
	uint32_t date;

	/* Size in dwords (CSS_Headerlen + PackageHeaderLen + dmc FWsLen)/4 */
	uint32_t size;

	/* Not used */
	uint32_t key_size;

	/* Not used */
	uint32_t modulus_size;

	/* Not used */
	uint32_t exponent_size;

	/* Not used */
	uint32_t reserved1[12];

	/* Major Minor */
	uint32_t version;

	/* Not used */
	uint32_t reserved2[8];

	/* Not used */
	uint32_t kernel_header_info;
} __packed;

struct intel_fw_info {
	uint16_t reserved1;

	/* Stepping (A, B, C, ..., *). * is a wildcard */
	char stepping;

	/* Sub-stepping (0, 1, ..., *). * is a wildcard */
	char substepping;

	uint32_t offset;
	uint32_t reserved2;
} __packed;

struct intel_package_header {
	/* DMC container header length in dwords */
	unsigned char header_len;

	/* always value would be 0x01 */
	unsigned char header_ver;

	unsigned char reserved[10];

	/* Number of valid entries in the FWInfo array below */
	uint32_t num_entries;

	struct intel_fw_info fw_info[20];
} __packed;

struct intel_dmc_header {
	/* always value would be 0x40403E3E */
	uint32_t signature;

	/* DMC binary header length */
	unsigned char header_len;

	/* 0x01 */
	unsigned char header_ver;

	/* Reserved */
	uint16_t dmcc_ver;

	/* Major, Minor */
	uint32_t	project;

	/* Firmware program size (excluding header) in dwords */
	uint32_t	fw_size;

	/* Major Minor version */
	uint32_t fw_version;

	/* Number of valid MMIO cycles present. */
	uint32_t mmio_count;

	/* MMIO address */
	uint32_t mmioaddr[8];

	/* MMIO data */
	uint32_t mmiodata[8];

	/* FW filename  */
	unsigned char dfile[32];

	uint32_t reserved1[2];
} __packed;

struct stepping_info {
	char stepping;
	char substepping;
};

<<<<<<< HEAD
static const struct stepping_info skl_stepping_info[] = {
		{'A', '0'}, {'B', '0'}, {'C', '0'},
		{'D', '0'}, {'E', '0'}, {'F', '0'},
		{'G', '0'}, {'H', '0'}, {'I', '0'},
		{'J', '0'}, {'K', '0'}
};

static struct stepping_info bxt_stepping_info[] = {
=======
static const struct stepping_info kbl_stepping_info[] = {
	{'A', '0'}, {'B', '0'}, {'C', '0'},
	{'D', '0'}, {'E', '0'}, {'F', '0'},
	{'G', '0'}, {'H', '0'}, {'I', '0'},
};

static const struct stepping_info skl_stepping_info[] = {
	{'A', '0'}, {'B', '0'}, {'C', '0'},
	{'D', '0'}, {'E', '0'}, {'F', '0'},
	{'G', '0'}, {'H', '0'}, {'I', '0'},
	{'J', '0'}, {'K', '0'}
};

static const struct stepping_info bxt_stepping_info[] = {
>>>>>>> v4.9.227
	{'A', '0'}, {'A', '1'}, {'A', '2'},
	{'B', '0'}, {'B', '1'}, {'B', '2'}
};

<<<<<<< HEAD
static char intel_get_stepping(struct drm_device *dev)
{
	if (IS_SKYLAKE(dev) && (dev->pdev->revision <
			ARRAY_SIZE(skl_stepping_info)))
		return skl_stepping_info[dev->pdev->revision].stepping;
	else if (IS_BROXTON(dev) && (dev->pdev->revision <
				ARRAY_SIZE(bxt_stepping_info)))
		return bxt_stepping_info[dev->pdev->revision].stepping;
	else
		return -ENODATA;
}

static char intel_get_substepping(struct drm_device *dev)
{
	if (IS_SKYLAKE(dev) && (dev->pdev->revision <
			ARRAY_SIZE(skl_stepping_info)))
		return skl_stepping_info[dev->pdev->revision].substepping;
	else if (IS_BROXTON(dev) && (dev->pdev->revision <
			ARRAY_SIZE(bxt_stepping_info)))
		return bxt_stepping_info[dev->pdev->revision].substepping;
	else
		return -ENODATA;
}

/**
 * intel_csr_load_status_get() - to get firmware loading status.
 * @dev_priv: i915 device.
 *
 * This function helps to get the firmware loading status.
 *
 * Return: Firmware loading status.
 */
enum csr_state intel_csr_load_status_get(struct drm_i915_private *dev_priv)
{
	enum csr_state state;

	mutex_lock(&dev_priv->csr_lock);
	state = dev_priv->csr.state;
	mutex_unlock(&dev_priv->csr_lock);

	return state;
}

/**
 * intel_csr_load_status_set() - help to set firmware loading status.
 * @dev_priv: i915 device.
 * @state: enumeration of firmware loading status.
 *
 * Set the firmware loading status.
 */
void intel_csr_load_status_set(struct drm_i915_private *dev_priv,
			enum csr_state state)
{
	mutex_lock(&dev_priv->csr_lock);
	dev_priv->csr.state = state;
	mutex_unlock(&dev_priv->csr_lock);
=======
static const struct stepping_info no_stepping_info = { '*', '*' };

static const struct stepping_info *
intel_get_stepping_info(struct drm_i915_private *dev_priv)
{
	const struct stepping_info *si;
	unsigned int size;

	if (IS_KABYLAKE(dev_priv)) {
		size = ARRAY_SIZE(kbl_stepping_info);
		si = kbl_stepping_info;
	} else if (IS_SKYLAKE(dev_priv)) {
		size = ARRAY_SIZE(skl_stepping_info);
		si = skl_stepping_info;
	} else if (IS_BROXTON(dev_priv)) {
		size = ARRAY_SIZE(bxt_stepping_info);
		si = bxt_stepping_info;
	} else {
		size = 0;
	}

	if (INTEL_REVID(dev_priv) < size)
		return si + INTEL_REVID(dev_priv);

	return &no_stepping_info;
}

static void gen9_set_dc_state_debugmask(struct drm_i915_private *dev_priv)
{
	uint32_t val, mask;

	mask = DC_STATE_DEBUG_MASK_MEMORY_UP;

	if (IS_BROXTON(dev_priv))
		mask |= DC_STATE_DEBUG_MASK_CORES;

	/* The below bit doesn't need to be cleared ever afterwards */
	val = I915_READ(DC_STATE_DEBUG);
	if ((val & mask) != mask) {
		val |= mask;
		I915_WRITE(DC_STATE_DEBUG, val);
		POSTING_READ(DC_STATE_DEBUG);
	}
>>>>>>> v4.9.227
}

/**
 * intel_csr_load_program() - write the firmware from memory to register.
<<<<<<< HEAD
 * @dev: drm device.
=======
 * @dev_priv: i915 drm device.
>>>>>>> v4.9.227
 *
 * CSR firmware is read from a .bin file and kept in internal memory one time.
 * Everytime display comes back from low power state this function is called to
 * copy the firmware from internal memory to registers.
 */
<<<<<<< HEAD
void intel_csr_load_program(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	u32 *payload = dev_priv->csr.dmc_payload;
	uint32_t i, fw_size;

	if (!IS_GEN9(dev)) {
=======
void intel_csr_load_program(struct drm_i915_private *dev_priv)
{
	u32 *payload = dev_priv->csr.dmc_payload;
	uint32_t i, fw_size;

	if (!IS_GEN9(dev_priv)) {
>>>>>>> v4.9.227
		DRM_ERROR("No CSR support available for this platform\n");
		return;
	}

<<<<<<< HEAD
	/*
	 * FIXME: Firmware gets lost on S3/S4, but not when entering system
	 * standby or suspend-to-idle (which is just like forced runtime pm).
	 * Unfortunately the ACPI subsystem doesn't yet give us a way to
	 * differentiate this, hence figure it out with this hack.
	 */
	if (I915_READ(CSR_PROGRAM(0)))
		return;

	mutex_lock(&dev_priv->csr_lock);
=======
	if (!dev_priv->csr.dmc_payload) {
		DRM_ERROR("Tried to program CSR with empty payload\n");
		return;
	}

>>>>>>> v4.9.227
	fw_size = dev_priv->csr.dmc_fw_size;
	for (i = 0; i < fw_size; i++)
		I915_WRITE(CSR_PROGRAM(i), payload[i]);

	for (i = 0; i < dev_priv->csr.mmio_count; i++) {
		I915_WRITE(dev_priv->csr.mmioaddr[i],
<<<<<<< HEAD
			dev_priv->csr.mmiodata[i]);
	}

	dev_priv->csr.state = FW_LOADED;
	mutex_unlock(&dev_priv->csr_lock);
}

static void finish_csr_load(const struct firmware *fw, void *context)
{
	struct drm_i915_private *dev_priv = context;
	struct drm_device *dev = dev_priv->dev;
=======
			   dev_priv->csr.mmiodata[i]);
	}

	dev_priv->csr.dc_state = 0;

	gen9_set_dc_state_debugmask(dev_priv);
}

static uint32_t *parse_csr_fw(struct drm_i915_private *dev_priv,
			      const struct firmware *fw)
{
>>>>>>> v4.9.227
	struct intel_css_header *css_header;
	struct intel_package_header *package_header;
	struct intel_dmc_header *dmc_header;
	struct intel_csr *csr = &dev_priv->csr;
<<<<<<< HEAD
	char stepping = intel_get_stepping(dev);
	char substepping = intel_get_substepping(dev);
	uint32_t dmc_offset = CSR_DEFAULT_FW_OFFSET, readcount = 0, nbytes;
	uint32_t i;
	uint32_t *dmc_payload;
	bool fw_loaded = false;

	if (!fw) {
		i915_firmware_load_error_print(csr->fw_path, 0);
		goto out;
	}

	if ((stepping == -ENODATA) || (substepping == -ENODATA)) {
		DRM_ERROR("Unknown stepping info, firmware loading failed\n");
		goto out;
	}
=======
	const struct stepping_info *si = intel_get_stepping_info(dev_priv);
	uint32_t dmc_offset = CSR_DEFAULT_FW_OFFSET, readcount = 0, nbytes;
	uint32_t i;
	uint32_t *dmc_payload;
	uint32_t required_version;
	size_t fsize;

	if (!fw)
		return NULL;

	fsize = sizeof(struct intel_css_header) +
		sizeof(struct intel_package_header) +
		sizeof(struct intel_dmc_header);
	if (fsize > fw->size)
		goto error_truncated;
>>>>>>> v4.9.227

	/* Extract CSS Header information*/
	css_header = (struct intel_css_header *)fw->data;
	if (sizeof(struct intel_css_header) !=
<<<<<<< HEAD
		(css_header->header_len * 4)) {
		DRM_ERROR("Firmware has wrong CSS header length %u bytes\n",
			(css_header->header_len * 4));
		goto out;
	}
=======
	    (css_header->header_len * 4)) {
		DRM_ERROR("Firmware has wrong CSS header length %u bytes\n",
			  (css_header->header_len * 4));
		return NULL;
	}

	csr->version = css_header->version;

	if (IS_KABYLAKE(dev_priv)) {
		required_version = KBL_CSR_VERSION_REQUIRED;
	} else if (IS_SKYLAKE(dev_priv)) {
		required_version = SKL_CSR_VERSION_REQUIRED;
	} else if (IS_BROXTON(dev_priv)) {
		required_version = BXT_CSR_VERSION_REQUIRED;
	} else {
		MISSING_CASE(INTEL_REVID(dev_priv));
		required_version = 0;
	}

	if (csr->version != required_version) {
		DRM_INFO("Refusing to load DMC firmware v%u.%u,"
			 " please use v%u.%u [" FIRMWARE_URL "].\n",
			 CSR_VERSION_MAJOR(csr->version),
			 CSR_VERSION_MINOR(csr->version),
			 CSR_VERSION_MAJOR(required_version),
			 CSR_VERSION_MINOR(required_version));
		return NULL;
	}

>>>>>>> v4.9.227
	readcount += sizeof(struct intel_css_header);

	/* Extract Package Header information*/
	package_header = (struct intel_package_header *)
<<<<<<< HEAD
					&fw->data[readcount];
	if (sizeof(struct intel_package_header) !=
		(package_header->header_len * 4)) {
		DRM_ERROR("Firmware has wrong package header length %u bytes\n",
			(package_header->header_len * 4));
		goto out;
=======
		&fw->data[readcount];
	if (sizeof(struct intel_package_header) !=
	    (package_header->header_len * 4)) {
		DRM_ERROR("Firmware has wrong package header length %u bytes\n",
			  (package_header->header_len * 4));
		return NULL;
>>>>>>> v4.9.227
	}
	readcount += sizeof(struct intel_package_header);

	/* Search for dmc_offset to find firware binary. */
	for (i = 0; i < package_header->num_entries; i++) {
		if (package_header->fw_info[i].substepping == '*' &&
<<<<<<< HEAD
			stepping == package_header->fw_info[i].stepping) {
			dmc_offset = package_header->fw_info[i].offset;
			break;
		} else if (stepping == package_header->fw_info[i].stepping &&
			substepping == package_header->fw_info[i].substepping) {
			dmc_offset = package_header->fw_info[i].offset;
			break;
		} else if (package_header->fw_info[i].stepping == '*' &&
			package_header->fw_info[i].substepping == '*')
			dmc_offset = package_header->fw_info[i].offset;
	}
	if (dmc_offset == CSR_DEFAULT_FW_OFFSET) {
		DRM_ERROR("Firmware not supported for %c stepping\n", stepping);
		goto out;
	}
	readcount += dmc_offset;
=======
		    si->stepping == package_header->fw_info[i].stepping) {
			dmc_offset = package_header->fw_info[i].offset;
			break;
		} else if (si->stepping == package_header->fw_info[i].stepping &&
			   si->substepping == package_header->fw_info[i].substepping) {
			dmc_offset = package_header->fw_info[i].offset;
			break;
		} else if (package_header->fw_info[i].stepping == '*' &&
			   package_header->fw_info[i].substepping == '*')
			dmc_offset = package_header->fw_info[i].offset;
	}
	if (dmc_offset == CSR_DEFAULT_FW_OFFSET) {
		DRM_ERROR("Firmware not supported for %c stepping\n",
			  si->stepping);
		return NULL;
	}
	readcount += dmc_offset;
	fsize += dmc_offset;
	if (fsize > fw->size)
		goto error_truncated;
>>>>>>> v4.9.227

	/* Extract dmc_header information. */
	dmc_header = (struct intel_dmc_header *)&fw->data[readcount];
	if (sizeof(struct intel_dmc_header) != (dmc_header->header_len)) {
		DRM_ERROR("Firmware has wrong dmc header length %u bytes\n",
<<<<<<< HEAD
				(dmc_header->header_len));
		goto out;
=======
			  (dmc_header->header_len));
		return NULL;
>>>>>>> v4.9.227
	}
	readcount += sizeof(struct intel_dmc_header);

	/* Cache the dmc header info. */
	if (dmc_header->mmio_count > ARRAY_SIZE(csr->mmioaddr)) {
		DRM_ERROR("Firmware has wrong mmio count %u\n",
<<<<<<< HEAD
						dmc_header->mmio_count);
		goto out;
=======
			  dmc_header->mmio_count);
		return NULL;
>>>>>>> v4.9.227
	}
	csr->mmio_count = dmc_header->mmio_count;
	for (i = 0; i < dmc_header->mmio_count; i++) {
		if (dmc_header->mmioaddr[i] < CSR_MMIO_START_RANGE ||
<<<<<<< HEAD
			dmc_header->mmioaddr[i] > CSR_MMIO_END_RANGE) {
			DRM_ERROR(" Firmware has wrong mmio address 0x%x\n",
						dmc_header->mmioaddr[i]);
			goto out;
		}
		csr->mmioaddr[i] = dmc_header->mmioaddr[i];
=======
		    dmc_header->mmioaddr[i] > CSR_MMIO_END_RANGE) {
			DRM_ERROR(" Firmware has wrong mmio address 0x%x\n",
				  dmc_header->mmioaddr[i]);
			return NULL;
		}
		csr->mmioaddr[i] = _MMIO(dmc_header->mmioaddr[i]);
>>>>>>> v4.9.227
		csr->mmiodata[i] = dmc_header->mmiodata[i];
	}

	/* fw_size is in dwords, so multiplied by 4 to convert into bytes. */
	nbytes = dmc_header->fw_size * 4;
<<<<<<< HEAD
	if (nbytes > CSR_MAX_FW_SIZE) {
		DRM_ERROR("CSR firmware too big (%u) bytes\n", nbytes);
		goto out;
	}
	csr->dmc_fw_size = dmc_header->fw_size;

	csr->dmc_payload = kmalloc(nbytes, GFP_KERNEL);
	if (!csr->dmc_payload) {
		DRM_ERROR("Memory allocation failed for dmc payload\n");
		goto out;
	}

	dmc_payload = csr->dmc_payload;
	memcpy(dmc_payload, &fw->data[readcount], nbytes);

	/* load csr program during system boot, as needed for DC states */
	intel_csr_load_program(dev);
	fw_loaded = true;

	DRM_DEBUG_KMS("Finished loading %s\n", dev_priv->csr.fw_path);
out:
	if (fw_loaded)
		intel_runtime_pm_put(dev_priv);
	else
		intel_csr_load_status_set(dev_priv, FW_FAILED);
=======
	fsize += nbytes;
	if (fsize > fw->size)
		goto error_truncated;

	if (nbytes > CSR_MAX_FW_SIZE) {
		DRM_ERROR("CSR firmware too big (%u) bytes\n", nbytes);
		return NULL;
	}
	csr->dmc_fw_size = dmc_header->fw_size;

	dmc_payload = kmalloc(nbytes, GFP_KERNEL);
	if (!dmc_payload) {
		DRM_ERROR("Memory allocation failed for dmc payload\n");
		return NULL;
	}

	return memcpy(dmc_payload, &fw->data[readcount], nbytes);

error_truncated:
	DRM_ERROR("Truncated DMC firmware, rejecting.\n");
	return NULL;
}

static void csr_load_work_fn(struct work_struct *work)
{
	struct drm_i915_private *dev_priv;
	struct intel_csr *csr;
	const struct firmware *fw;
	int ret;

	dev_priv = container_of(work, typeof(*dev_priv), csr.work);
	csr = &dev_priv->csr;

	ret = request_firmware(&fw, dev_priv->csr.fw_path,
			       &dev_priv->drm.pdev->dev);
	if (fw)
		dev_priv->csr.dmc_payload = parse_csr_fw(dev_priv, fw);

	if (dev_priv->csr.dmc_payload) {
		intel_csr_load_program(dev_priv);

		intel_display_power_put(dev_priv, POWER_DOMAIN_INIT);

		DRM_INFO("Finished loading %s (v%u.%u)\n",
			 dev_priv->csr.fw_path,
			 CSR_VERSION_MAJOR(csr->version),
			 CSR_VERSION_MINOR(csr->version));
	} else {
		dev_notice(dev_priv->drm.dev,
			   "Failed to load DMC firmware"
			   " [" FIRMWARE_URL "],"
			   " disabling runtime power management.\n");
	}
>>>>>>> v4.9.227

	release_firmware(fw);
}

/**
 * intel_csr_ucode_init() - initialize the firmware loading.
<<<<<<< HEAD
 * @dev: drm device.
=======
 * @dev_priv: i915 drm device.
>>>>>>> v4.9.227
 *
 * This function is called at the time of loading the display driver to read
 * firmware from a .bin file and copied into a internal memory.
 */
<<<<<<< HEAD
void intel_csr_ucode_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_csr *csr = &dev_priv->csr;
	int ret;

	if (!HAS_CSR(dev))
		return;

	if (IS_SKYLAKE(dev))
=======
void intel_csr_ucode_init(struct drm_i915_private *dev_priv)
{
	struct intel_csr *csr = &dev_priv->csr;

	INIT_WORK(&dev_priv->csr.work, csr_load_work_fn);

	if (!HAS_CSR(dev_priv))
		return;

	if (IS_KABYLAKE(dev_priv))
		csr->fw_path = I915_CSR_KBL;
	else if (IS_SKYLAKE(dev_priv))
>>>>>>> v4.9.227
		csr->fw_path = I915_CSR_SKL;
	else if (IS_BROXTON(dev_priv))
		csr->fw_path = I915_CSR_BXT;
	else {
		DRM_ERROR("Unexpected: no known CSR firmware for platform\n");
<<<<<<< HEAD
		intel_csr_load_status_set(dev_priv, FW_FAILED);
=======
>>>>>>> v4.9.227
		return;
	}

	DRM_DEBUG_KMS("Loading %s\n", csr->fw_path);

	/*
	 * Obtain a runtime pm reference, until CSR is loaded,
	 * to avoid entering runtime-suspend.
	 */
<<<<<<< HEAD
	intel_runtime_pm_get(dev_priv);

	/* CSR supported for platform, load firmware */
	ret = request_firmware_nowait(THIS_MODULE, true, csr->fw_path,
				&dev_priv->dev->pdev->dev,
				GFP_KERNEL, dev_priv,
				finish_csr_load);
	if (ret) {
		i915_firmware_load_error_print(csr->fw_path, ret);
		intel_csr_load_status_set(dev_priv, FW_FAILED);
	}
=======
	intel_display_power_get(dev_priv, POWER_DOMAIN_INIT);

	schedule_work(&dev_priv->csr.work);
}

/**
 * intel_csr_ucode_suspend() - prepare CSR firmware before system suspend
 * @dev_priv: i915 drm device
 *
 * Prepare the DMC firmware before entering system suspend. This includes
 * flushing pending work items and releasing any resources acquired during
 * init.
 */
void intel_csr_ucode_suspend(struct drm_i915_private *dev_priv)
{
	if (!HAS_CSR(dev_priv))
		return;

	flush_work(&dev_priv->csr.work);

	/* Drop the reference held in case DMC isn't loaded. */
	if (!dev_priv->csr.dmc_payload)
		intel_display_power_put(dev_priv, POWER_DOMAIN_INIT);
}

/**
 * intel_csr_ucode_resume() - init CSR firmware during system resume
 * @dev_priv: i915 drm device
 *
 * Reinitialize the DMC firmware during system resume, reacquiring any
 * resources released in intel_csr_ucode_suspend().
 */
void intel_csr_ucode_resume(struct drm_i915_private *dev_priv)
{
	if (!HAS_CSR(dev_priv))
		return;

	/*
	 * Reacquire the reference to keep RPM disabled in case DMC isn't
	 * loaded.
	 */
	if (!dev_priv->csr.dmc_payload)
		intel_display_power_get(dev_priv, POWER_DOMAIN_INIT);
>>>>>>> v4.9.227
}

/**
 * intel_csr_ucode_fini() - unload the CSR firmware.
<<<<<<< HEAD
 * @dev: drm device.
 *
 * Firmmware unloading includes freeing the internal momory and reset the
 * firmware loading status.
 */
void intel_csr_ucode_fini(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	if (!HAS_CSR(dev))
		return;

	intel_csr_load_status_set(dev_priv, FW_FAILED);
	kfree(dev_priv->csr.dmc_payload);
}

void assert_csr_loaded(struct drm_i915_private *dev_priv)
{
	WARN_ONCE(intel_csr_load_status_get(dev_priv) != FW_LOADED,
		  "CSR is not loaded.\n");
	WARN_ONCE(!I915_READ(CSR_PROGRAM(0)),
		  "CSR program storage start is NULL\n");
	WARN_ONCE(!I915_READ(CSR_SSP_BASE), "CSR SSP Base Not fine\n");
	WARN_ONCE(!I915_READ(CSR_HTP_SKL), "CSR HTP Not fine\n");
}
=======
 * @dev_priv: i915 drm device.
 *
 * Firmmware unloading includes freeing the internal memory and reset the
 * firmware loading status.
 */
void intel_csr_ucode_fini(struct drm_i915_private *dev_priv)
{
	if (!HAS_CSR(dev_priv))
		return;

	intel_csr_ucode_suspend(dev_priv);

	kfree(dev_priv->csr.dmc_payload);
}
>>>>>>> v4.9.227
