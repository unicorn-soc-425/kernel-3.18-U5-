/// PTR_ERR should access the value just tested by IS_ERR
<<<<<<< HEAD
//# There can be false positives in the patch case, where it is the call
//# IS_ERR that is wrong.
///
// Confidence: High
// Copyright: (C) 2012 Julia Lawall, INRIA.  GPLv2.
// Copyright: (C) 2012 Gilles Muller, INRIA.  GPLv2.
// URL: http://coccinelle.lip6.fr/
// Comments:
=======
//# There can be false positives in the patch case, where it is the call to
//# IS_ERR that is wrong.
///
// Confidence: High
// Copyright: (C) 2012, 2015 Julia Lawall, INRIA.  GPLv2.
// Copyright: (C) 2012, 2015 Gilles Muller, INRIA.  GPLv2.
// URL: http://coccinelle.lip6.fr/
>>>>>>> v4.9.227
// Options: --no-includes --include-headers

virtual patch
virtual context
virtual org
virtual report

<<<<<<< HEAD
@depends on patch@
expression e,e1;
@@

(
if (IS_ERR(e)) { ... PTR_ERR(e) ... }
|
if (IS_ERR(e=e1)) { ... PTR_ERR(e) ... }
|
if (IS_ERR(e))
 { ...
  PTR_ERR(
-   e1
+   e
  )
   ... }
)

@r depends on !patch@
expression e,e1;
position p1,p2;
@@

(
if (IS_ERR(e)) { ... PTR_ERR(e) ... }
|
if (IS_ERR(e=e1)) { ... PTR_ERR(e) ... }
|
*if (IS_ERR@p1(e))
 { ...
*  PTR_ERR@p2(e1)
   ... }
)

@script:python depends on org@
p1 << r.p1;
p2 << r.p2;
@@

cocci.print_main("inconsistent IS_ERR and PTR_ERR",p1)
cocci.print_secs("PTR_ERR",p2)

@script:python depends on report@
p1 << r.p1;
p2 << r.p2;
@@

msg = "inconsistent IS_ERR and PTR_ERR, PTR_ERR on line %s" % (p2[0].line)
coccilib.report.print_report(p1[0],msg)
=======
@ok1 exists@
expression x,e;
position p;
@@

if (IS_ERR(x=e) || ...) {
  <...
   PTR_ERR@p(x)
  ...>
}

@ok2 exists@
expression x,e1,e2;
position p;
@@

if (IS_ERR(x) || ...) {
  <...
(
   PTR_ERR@p(\(e1 ? e2 : x\|e1 ? x : e2\))
|
   PTR_ERR@p(x)
)
  ...>
}

@r1 depends on patch && !context && !org && !report exists@
expression x,y;
position p != {ok1.p,ok2.p};
@@

if (IS_ERR(x) || ...) {
  ... when any
      when != IS_ERR(...)
(
  PTR_ERR(x)
|
  PTR_ERR@p(
-     y
+     x
  )
)
  ... when any
}

// ----------------------------------------------------------------------------

@r1_context depends on !patch && (context || org || report) exists@
position p != {ok1.p,ok2.p};
expression x, y;
position j0, j1;
@@

if (IS_ERR@j0(x) || ...) {
  ... when any
      when != IS_ERR(...)
(
  PTR_ERR(x)
|
  PTR_ERR@j1@p(
     y
  )
)
  ... when any
}

@r1_disj depends on !patch && (context || org || report) exists@
position p != {ok1.p,ok2.p};
expression x, y;
position r1_context.j0, r1_context.j1;
@@

* if (IS_ERR@j0(x) || ...) {
  ... when any
      when != IS_ERR(...)
*   PTR_ERR@j1@p(
     y
  )
  ... when any
}

// ----------------------------------------------------------------------------

@script:python r1_org depends on org@
j0 << r1_context.j0;
j1 << r1_context.j1;
@@

msg = "inconsistent IS_ERR and PTR_ERR"
coccilib.org.print_todo(j0[0], msg)
coccilib.org.print_link(j1[0], "")

// ----------------------------------------------------------------------------

@script:python r1_report depends on report@
j0 << r1_context.j0;
j1 << r1_context.j1;
@@

msg = "inconsistent IS_ERR and PTR_ERR on line %s." % (j1[0].line)
coccilib.report.print_report(j0[0], msg)

>>>>>>> v4.9.227
