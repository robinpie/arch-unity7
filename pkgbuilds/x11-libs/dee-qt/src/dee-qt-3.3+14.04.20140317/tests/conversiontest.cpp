/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtTest>
#include <QObject>

#include "deelistmodel.h"

#include <dee.h>

class ConversionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
      g_type_init();
    }

    void ModelQVariantConversionTest()
    {
      DeeModel* model = dee_sequence_model_new();
      dee_model_set_schema(model, "b", "y", "n", "q", "i", "u", "x", "t", "d", "s", "a(ii)", "a{sv}", NULL);

      GVariant **tuples = g_new(GVariant *, 2);

      GVariant **t1 = g_new(GVariant *, 2);
      t1[0] = g_variant_new_int32(1);
      t1[1] = g_variant_new_int32(2);
      tuples[0] = g_variant_new_tuple(t1, 2);

      GVariant **t2 = g_new(GVariant *, 2);
      t2[0] = g_variant_new_int32(3);
      t2[1] = g_variant_new_int32(4);
      tuples[1] = g_variant_new_tuple(t2, 2);

      GVariant* array_of_tuples = g_variant_new_array(((const GVariantType *) "(ii)"), tuples, 2);

      // populate hints a{sv}
      int cnt = 0;
      GVariant **children = new GVariant*[3];
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("metallica"), g_variant_new_variant(g_variant_new_int32(1982)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("am i evil?"), g_variant_new_variant(g_variant_new_boolean(true)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("master of"), g_variant_new_variant(g_variant_new_string("puppets")));
      GVariant* hints = g_variant_new_array(G_VARIANT_TYPE("{sv}"), children, cnt);

      dee_model_append(model, TRUE, 7, INT16_MIN, UINT16_MAX, INT32_MIN, UINT32_MAX, INT64_MIN, UINT64_MAX, 3.1415, "giraffe", array_of_tuples, hints);

      DeeListModel model_qt;
      QCOMPARE(model_qt.count(), 0);

      model_qt.setModel(model);
      QCOMPARE(model_qt.synchronized(), true);
      QCOMPARE(model_qt.count(), 1);

      const QModelIndex row0Index = model_qt.index(0, 0);
      QCOMPARE(model_qt.data(row0Index, 0), QVariant(true));
      QCOMPARE(model_qt.data(row0Index, 1), QVariant(7));
      QCOMPARE(model_qt.data(row0Index, 2), QVariant(INT16_MIN));
      QCOMPARE(model_qt.data(row0Index, 3), QVariant(UINT16_MAX));
      QCOMPARE(model_qt.data(row0Index, 4), QVariant(INT32_MIN));
      QCOMPARE(model_qt.data(row0Index, 5), QVariant(UINT32_MAX));
      QCOMPARE(model_qt.data(row0Index, 6), QVariant((qint64)INT64_MIN));
      QCOMPARE(model_qt.data(row0Index, 7), QVariant((quint64)UINT64_MAX));
      QCOMPARE(model_qt.data(row0Index, 8), QVariant(3.1415));
      QCOMPARE(model_qt.data(row0Index, 9), QVariant("giraffe"));

      QList< QVariant > expected_array;
      QList< QVariant > tuple1, tuple2;
      tuple1 << 1 << 2;
      tuple2 << 3 << 4;
      expected_array << QVariant(tuple1) << QVariant(tuple2);
      QCOMPARE(model_qt.data(row0Index, 10), QVariant(expected_array));

      QVariantHash res = model_qt.data(row0Index, 11).toHash();
      QCOMPARE(res["metallica"], QVariant(1982));
      QCOMPARE(res["am i evil?"], QVariant(true));
      QCOMPARE(res["master of"], QVariant("puppets"));

      g_free(tuples);
      g_free(t1);
      g_free(t2);
      g_object_unref(model);
    }

    void GVariantToQVariantConversionTest()
    {
      GVariant **tuples = g_new(GVariant *, 2);

      GVariant **t1 = g_new(GVariant *, 2);
      t1[0] = g_variant_new_int32(1);
      t1[1] = g_variant_new_int32(2);
      tuples[0] = g_variant_new_tuple(t1, 2);

      GVariant **t2 = g_new(GVariant *, 2);
      t2[0] = g_variant_new_int32(3);
      t2[1] = g_variant_new_int32(4);
      tuples[1] = g_variant_new_tuple(t2, 2);

      GVariant* array_of_tuples = g_variant_new_array(((const GVariantType *) "(ii)"), tuples, 2);

      QVariant variant(DeeListModel::VariantForData(array_of_tuples));

      QList< QVariant > expected_array;
      QList< QVariant > tuple1, tuple2;
      tuple1 << 1 << 2;
      tuple2 << 3 << 4;
      expected_array << QVariant(tuple1) << QVariant(tuple2);
      QCOMPARE(variant, QVariant(expected_array));

      g_free(t1);
      g_free(t2);
      g_free(tuples);

      GVariant* nested_variant = g_variant_new_variant(g_variant_new_string("nested"));
      variant = DeeListModel::VariantForData(nested_variant);
      QCOMPARE(variant.toString(), QString("nested"));
    }

    void QVariantToGVariantConversionTest()
    {
      int cnt = 0;
      GVariant **children = new GVariant*[8];
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("int"), g_variant_new_variant(g_variant_new_int32(-82)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("bool"), g_variant_new_variant(g_variant_new_boolean(TRUE)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("string"), g_variant_new_variant(g_variant_new_string("foo")));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("uint"), g_variant_new_variant(g_variant_new_uint32(90)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("int64"), g_variant_new_variant(g_variant_new_int64(-401230)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("uint64"), g_variant_new_variant(g_variant_new_uint64(401230)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("double"), g_variant_new_variant(g_variant_new_double(3.1415)));
      children[cnt++] = g_variant_new_dict_entry(g_variant_new_string("int-array"), g_variant_new_variant(g_variant_new_parsed("[1, 2, 3]")));
      GVariant* hints = g_variant_new_array(G_VARIANT_TYPE("{sv}"), children, cnt);

      QVariant variant(DeeListModel::VariantForData(hints));
      GVariant* reconstructed_gvariant = DeeListModel::DataFromVariant(variant);

      // the ordering in the variant might be different, so don't compare directly
      QVERIFY(g_variant_type_equal(g_variant_get_type(hints), g_variant_get_type(reconstructed_gvariant)));
      QCOMPARE(g_variant_n_children(hints), g_variant_n_children(reconstructed_gvariant));
    }
};

QTEST_MAIN(ConversionTest)

#include "conversiontest.moc"
