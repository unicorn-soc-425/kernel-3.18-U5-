/// Make sure pm_runtime_* calls does not use unnecessary IS_ERR_VALUE
<<<<<<< HEAD
//
=======
///
>>>>>>> v4.9.227
// Keywords: pm_runtime
// Confidence: Medium
// Copyright (C) 2013 Texas Instruments Incorporated - GPLv2.
// URL: http://coccinelle.lip6.fr/
// Options: --include-headers

virtual patch
virtual context
virtual org
virtual report

//----------------------------------------------------------
//  Detection
//----------------------------------------------------------

@runtime_bad_err_handle exists@
expression ret;
<<<<<<< HEAD
@@
(
ret = \(pm_runtime_idle\|
=======
position p;
@@
(
ret@p = \(pm_runtime_idle\|
>>>>>>> v4.9.227
	pm_runtime_suspend\|
	pm_runtime_autosuspend\|
	pm_runtime_resume\|
	pm_request_idle\|
	pm_request_resume\|
	pm_request_autosuspend\|
	pm_runtime_get\|
	pm_runtime_get_sync\|
	pm_runtime_put\|
	pm_runtime_put_autosuspend\|
	pm_runtime_put_sync\|
	pm_runtime_put_sync_suspend\|
	pm_runtime_put_sync_autosuspend\|
	pm_runtime_set_active\|
	pm_schedule_suspend\|
	pm_runtime_barrier\|
	pm_generic_runtime_suspend\|
	pm_generic_runtime_resume\)(...);
...
IS_ERR_VALUE(ret)
...
)

//----------------------------------------------------------
//  For context mode
//----------------------------------------------------------

<<<<<<< HEAD
@depends on runtime_bad_err_handle && context@
identifier pm_runtime_api;
expression ret;
@@
(
ret = pm_runtime_api(...);
=======
@depends on context@
identifier pm_runtime_api;
expression ret;
position runtime_bad_err_handle.p;
@@
(
ret@p = pm_runtime_api(...);
>>>>>>> v4.9.227
...
* IS_ERR_VALUE(ret)
...
)

//----------------------------------------------------------
//  For patch mode
//----------------------------------------------------------

<<<<<<< HEAD
@depends on runtime_bad_err_handle && patch@
identifier pm_runtime_api;
expression ret;
@@
(
ret = pm_runtime_api(...);
=======
@depends on patch@
identifier pm_runtime_api;
expression ret;
position runtime_bad_err_handle.p;
@@
(
ret@p = pm_runtime_api(...);
>>>>>>> v4.9.227
...
- IS_ERR_VALUE(ret)
+ ret < 0
...
)

//----------------------------------------------------------
//  For org and report mode
//----------------------------------------------------------

<<<<<<< HEAD
@r depends on runtime_bad_err_handle exists@
position p1, p2;
identifier pm_runtime_api;
expression ret;
@@
(
ret = pm_runtime_api@p1(...);
=======
@r depends on (org || report) exists@
position p1, p2;
identifier pm_runtime_api;
expression ret;
position runtime_bad_err_handle.p;
@@
(
ret@p = pm_runtime_api@p1(...);
>>>>>>> v4.9.227
...
IS_ERR_VALUE@p2(ret)
...
)

@script:python depends on org@
p1 << r.p1;
p2 << r.p2;
pm_runtime_api << r.pm_runtime_api;
@@

cocci.print_main(pm_runtime_api,p1)
cocci.print_secs("IS_ERR_VALUE",p2)

@script:python depends on report@
p1 << r.p1;
p2 << r.p2;
pm_runtime_api << r.pm_runtime_api;
@@

msg = "%s returns < 0 as error. Unecessary IS_ERR_VALUE at line %s" % (pm_runtime_api, p2[0].line)
coccilib.report.print_report(p1[0],msg)
