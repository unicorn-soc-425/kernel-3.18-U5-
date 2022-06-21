<<<<<<< HEAD
/// Make sure of_device_id tables are NULL terminated
//
// Keywords: of_table
=======
/// Make sure (of/i2c/platform)_device_id tables are NULL terminated
//
// Keywords: of_table i2c_table platform_table
>>>>>>> v4.9.227
// Confidence: Medium
// Options: --include-headers

virtual patch
virtual context
virtual org
virtual report

@depends on context@
identifier var, arr;
expression E;
@@
<<<<<<< HEAD
struct of_device_id arr[] = {
=======
(
struct \(of_device_id \| i2c_device_id \| platform_device_id\) arr[] = {
>>>>>>> v4.9.227
	...,
	{
	.var = E,
*	}
};
<<<<<<< HEAD
=======
|
struct \(of_device_id \| i2c_device_id \| platform_device_id\) arr[] = {
	...,
*	{ ..., E, ... },
};
)
>>>>>>> v4.9.227

@depends on patch@
identifier var, arr;
expression E;
@@
<<<<<<< HEAD
struct of_device_id arr[] = {
=======
(
struct \(of_device_id \| i2c_device_id \| platform_device_id\) arr[] = {
>>>>>>> v4.9.227
	...,
	{
	.var = E,
-	}
+	},
+	{ }
};
<<<<<<< HEAD
=======
|
struct \(of_device_id \| i2c_device_id \| platform_device_id\) arr[] = {
	...,
	{ ..., E, ... },
+	{ },
};
)
>>>>>>> v4.9.227

@r depends on org || report@
position p1;
identifier var, arr;
expression E;
@@
<<<<<<< HEAD
struct of_device_id arr[] = {
=======
(
struct \(of_device_id \| i2c_device_id \| platform_device_id\) arr[] = {
>>>>>>> v4.9.227
	...,
	{
	.var = E,
	}
	@p1
};
<<<<<<< HEAD
=======
|
struct \(of_device_id \| i2c_device_id \| platform_device_id\) arr[] = {
	...,
	{ ..., E, ... }
	@p1
};
)
>>>>>>> v4.9.227

@script:python depends on org@
p1 << r.p1;
arr << r.arr;
@@

cocci.print_main(arr,p1)

@script:python depends on report@
p1 << r.p1;
arr << r.arr;
@@

msg = "%s is not NULL terminated at line %s" % (arr, p1[0].line)
coccilib.report.print_report(p1[0],msg)
