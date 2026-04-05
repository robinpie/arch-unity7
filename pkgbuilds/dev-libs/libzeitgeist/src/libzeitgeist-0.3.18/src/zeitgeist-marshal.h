
#ifndef ___zeitgeist_cclosure_marshal_MARSHAL_H__
#define ___zeitgeist_cclosure_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,OBJECT (marshal.list:2) */
extern void _zeitgeist_cclosure_marshal_VOID__OBJECT_OBJECT (GClosure     *closure,
                                                             GValue       *return_value,
                                                             guint         n_param_values,
                                                             const GValue *param_values,
                                                             gpointer      invocation_hint,
                                                             gpointer      marshal_data);

/* VOID:OBJECT,BOXED (marshal.list:5) */
extern void _zeitgeist_cclosure_marshal_VOID__OBJECT_BOXED (GClosure     *closure,
                                                            GValue       *return_value,
                                                            guint         n_param_values,
                                                            const GValue *param_values,
                                                            gpointer      invocation_hint,
                                                            gpointer      marshal_data);

/* VOID:STRING,BOOLEAN (marshal.list:8) */
extern void _zeitgeist_cclosure_marshal_VOID__STRING_BOOLEAN (GClosure     *closure,
                                                              GValue       *return_value,
                                                              guint         n_param_values,
                                                              const GValue *param_values,
                                                              gpointer      invocation_hint,
                                                              gpointer      marshal_data);

G_END_DECLS

#endif /* ___zeitgeist_cclosure_marshal_MARSHAL_H__ */

