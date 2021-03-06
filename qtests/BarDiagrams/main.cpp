/**
 * Copyright (C) 2001-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
 *
 * This file is part of the KD Chart library.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QtTest/QtTest>
#include <KChartChart>
#include <KChartGlobal>
#include <KChartBarDiagram>
#include <KChartThreeDBarAttributes>
#include <KChartCartesianCoordinatePlane>

#include <TableModel.h>

using namespace KChart;

class TestBarDiagrams: public QObject {
    Q_OBJECT
private slots:

    void initTestCase()
    {
        m_chart = new Chart(nullptr);
        m_model = new TableModel( nullptr );
        m_model->loadFromCSV( ":/data" );
        m_bars = new BarDiagram();
        m_bars->setModel( m_model );
        m_chart->coordinatePlane()->replaceDiagram( m_bars );
    }

    void testBarDiagramType()
    {
        QCOMPARE( m_bars->type(), BarDiagram::Normal );
        m_bars->setType( BarDiagram::Stacked );
        QCOMPARE(  m_bars->type(),  BarDiagram::Stacked );
        m_bars->setType( BarDiagram::Percent );
        QCOMPARE(  m_bars->type(),  BarDiagram::Percent );
#ifdef TEMPORARILY_REMOVED
        m_bars->setType( BarDiagram::Rows );
        QCOMPARE(  m_bars->type(),  BarDiagram::Rows );
#endif
        //reset to normal
        m_bars->setType( BarDiagram::Normal );
    }

    void testBarAttributesLevelSettings()
    {
        //check segments
        const int rows = m_bars->model()->rowCount();
        QCOMPARE( m_bars->numberOfAbscissaSegments(), rows );
        const int cols = m_bars->model()->columnCount();
        QCOMPARE( m_bars->numberOfOrdinateSegments(), cols );
        QModelIndex idx = m_model->index(rows-3, cols-3, QModelIndex());
        // create attribut
        BarAttributes ba( m_bars->barAttributes() );
        BarAttributes baCol( m_bars->barAttributes() );
        BarAttributes baIndex( m_bars->barAttributes() );
        // modify at different level and compare
        baCol.setBarGapFactor(2 );
        baIndex.setBarGapFactor( 3 );
        m_bars->setBarAttributes( ba );
        m_bars->setBarAttributes( cols-2,  baCol );
        m_bars->setBarAttributes( idx,  baIndex );
        QVERIFY( m_bars->barAttributes() != m_bars->barAttributes(cols-2) );
        QVERIFY( m_bars->barAttributes() != m_bars->barAttributes(idx) );
        QVERIFY( m_bars->barAttributes(cols-2) != m_bars->barAttributes(idx) );
        QCOMPARE( m_bars->barAttributes(),  ba );
        QCOMPARE( m_bars->barAttributes( cols - 2 ),  baCol );
        QCOMPARE( m_bars->barAttributes( idx ),  baIndex );
        // try and override the cols and index level - should not work
        m_bars->setBarAttributes( ba );
        QVERIFY( m_bars->barAttributes().barGapFactor() == 0.4 );
        QVERIFY( m_bars->barAttributes( cols-2 ).barGapFactor() == 2 );
        QVERIFY( m_bars->barAttributes( idx ).barGapFactor() == 3 );
    }

    void testBarAttributesValueSettings()
    {
        BarAttributes ba( m_bars->barAttributes() );

        // check default values
        QVERIFY( ba.fixedDataValueGap() ==  6 );
        QVERIFY( ba.useFixedDataValueGap() == false );
        QVERIFY( ba.fixedValueBlockGap() ==  24 );
        QVERIFY( ba.useFixedValueBlockGap() == false );
        QVERIFY( ba.fixedBarWidth() ==  -1 );
        QVERIFY( ba.useFixedBarWidth() == false );
        QVERIFY( ba.drawSolidExcessArrows() == false );
        QVERIFY( ba.groupGapFactor() == 2.0 );
        QVERIFY( ba.barGapFactor() == 0.4 );
        //change settings
        ba.setFixedDataValueGap( 7 );
        ba.setUseFixedDataValueGap( true );
        ba.setFixedValueBlockGap( 25 );
        ba.setUseFixedValueBlockGap( true );
        ba.setFixedBarWidth( 1 );
        ba.setUseFixedBarWidth( true );
        ba.setDrawSolidExcessArrows(  true ); //not implemented yet
        ba.setGroupGapFactor( 2 );
        ba.setBarGapFactor( 1 );
        m_bars->setBarAttributes(  ba );
        // get new values
        QVERIFY( m_bars->barAttributes().fixedDataValueGap() ==  7 );
        QVERIFY( m_bars->barAttributes().useFixedDataValueGap() == true );
        QVERIFY( m_bars->barAttributes().fixedValueBlockGap() ==  25 );
        QVERIFY( m_bars->barAttributes().useFixedValueBlockGap() == true );
        QVERIFY( m_bars->barAttributes().fixedBarWidth() ==  1 );
        QVERIFY( m_bars->barAttributes().useFixedBarWidth() == true );
        QVERIFY( m_bars->barAttributes().drawSolidExcessArrows() == true );
        QVERIFY( m_bars->barAttributes().groupGapFactor() == 2 );
        QVERIFY( m_bars->barAttributes().barGapFactor() == 1 );
    }

        void testThreeDBarAttributesLevelSettings()
    {
        //check segments
        const int rows = m_bars->model()->rowCount();
        QCOMPARE( m_bars->numberOfAbscissaSegments(), rows );
        const int cols = m_bars->model()->columnCount();
        QCOMPARE( m_bars->numberOfOrdinateSegments(), cols );
        QModelIndex idx = m_model->index(rows-3, cols-3, QModelIndex());
        // create attribut
        ThreeDBarAttributes td( m_bars->threeDBarAttributes() );
        ThreeDBarAttributes tdCol( m_bars->threeDBarAttributes() );
        ThreeDBarAttributes tdIndex( m_bars->threeDBarAttributes() );
        // modify at different level and compare
        tdCol.setDepth(25 );
        tdIndex.setDepth( 30 );
        m_bars->setThreeDBarAttributes( td  );
        m_bars->setThreeDBarAttributes( cols-2,  tdCol );
        m_bars->setThreeDBarAttributes( idx,  tdIndex );
        QVERIFY( m_bars->threeDBarAttributes() !=
                 m_bars->threeDBarAttributes(cols-2) );
        QVERIFY( m_bars->threeDBarAttributes() !=
                 m_bars->threeDBarAttributes(idx) );
        QVERIFY( m_bars->threeDBarAttributes(cols-2) !=
                 m_bars->threeDBarAttributes(idx) );
        QCOMPARE( m_bars->threeDBarAttributes(),  td );
        QCOMPARE( m_bars->threeDBarAttributes( cols - 2 ),  tdCol );
        QCOMPARE( m_bars->threeDBarAttributes( idx ),  tdIndex );
        // try and override the cols and index level - should not work
        m_bars->setThreeDBarAttributes( td );
        QVERIFY( m_bars->threeDBarAttributes().depth() == 20 );
        QVERIFY( m_bars->threeDBarAttributes( cols-2 ).depth() == 25 );
        QVERIFY( m_bars->threeDBarAttributes( idx ).depth() == 30 );
    }

    void testThreeDBarAttributesValueSettings()
    {
        ThreeDBarAttributes td( m_bars->threeDBarAttributes() );

        //check default values
        //generics == AbstractThreeD
        QVERIFY( td.isEnabled() == false );
        QVERIFY( td.depth() == 20 );
        QVERIFY( td.validDepth() == 0.0 );
        //bars specifics
        QVERIFY( td.useShadowColors() == true ); // Not implemented
        QVERIFY( td.angle() == 45 ); // Not implemented

        //set new values
        td.setEnabled(  true );
        td.setDepth( 40 );
        td.setUseShadowColors( false );
        td.setAngle( 75 );
        m_bars->setThreeDBarAttributes( td );

        //get new values
        QVERIFY( m_bars->threeDBarAttributes().isEnabled() == true );
        QVERIFY( m_bars->threeDBarAttributes().depth() == 40 );
        QVERIFY( m_bars->threeDBarAttributes().validDepth() == 40 );
        QVERIFY( m_bars->threeDBarAttributes().useShadowColors() == false );
        QVERIFY( m_bars->threeDBarAttributes().angle() == 75 );
    }

    void cleanupTestCase()
    {
    }

private:
    Chart *m_chart;
    BarDiagram *m_bars;
    TableModel *m_model;

};

QTEST_MAIN(TestBarDiagrams)

#include "main.moc"
