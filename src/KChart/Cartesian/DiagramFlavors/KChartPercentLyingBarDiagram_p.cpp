/*
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

#include "KChartPercentLyingBarDiagram_p.h"

#include <QModelIndex>

#include "KChartBarDiagram.h"
#include "KChartTextAttributes.h"
#include "KChartAttributesModel.h"
#include "KChartAbstractCartesianDiagram.h"

using namespace KChart;

PercentLyingBarDiagram::PercentLyingBarDiagram( BarDiagram* d )
    : BarDiagramType( d )
{
}

BarDiagram::BarType PercentLyingBarDiagram::type() const
{
    return BarDiagram::Percent;
}

const QPair<QPointF, QPointF> PercentLyingBarDiagram::calculateDataBoundaries() const
{
    //const int rowCount = compressor().modelDataRows();
    //const int colCount = compressor().modelDataColumns();

    const qreal xMin = 0;
    const qreal xMax = diagram()->model() ? diagram()->model()->rowCount( diagram()->rootIndex() ) : 0;
    qreal yMin = 0.0, yMax = 100.0;
    /*for ( int col = 0; col < colCount; ++col )
    {
        for ( int row = 0; row < rowCount; ++row )
        {
            // Ordinate should begin at 0 the max value being the 100% pos
            const QModelIndex idx = diagram()->model()->index( row, col, diagram()->rootIndex() );
            // only positive values are handled
            qreal value = diagram()->model()->data( idx ).toReal();
            if ( value > 0 )
                yMax = qMax( yMax, value );
        }
    }*/
    // special cases
    if ( yMax == yMin ) {
        if ( yMin == 0.0 )
            yMax = 0.1; //we need at least a range
        else
            yMax = 0.0; // they are the same but negative
    }
    const QPointF bottomLeft( QPointF( yMin, xMin ) );
    const QPointF topRight( QPointF( yMax, xMax ) );

    //qDebug() << "BarDiagram::calculateDataBoundaries () returns ( " << bottomLeft << topRight <<")";
    return QPair< QPointF, QPointF >( bottomLeft,  topRight );
}

void PercentLyingBarDiagram::paint( PaintContext* ctx )
{
    reverseMapper().clear();

    const QPair<QPointF,QPointF> boundaries = diagram()->dataBoundaries(); // cached

    const QPointF boundLeft = ctx->coordinatePlane()->translate( boundaries.first ) ;
    const QPointF boundRight = ctx->coordinatePlane()->translate( boundaries.second );

    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();

    BarAttributes ba = diagram()->barAttributes();
    qreal barWidth = 0;
    qreal maxDepth = 0;
    qreal width = boundLeft.y() - boundRight.y();
    qreal groupWidth = width / rowCount;
    qreal spaceBetweenBars = 0;
    qreal spaceBetweenGroups = 0;

    if ( ba.useFixedBarWidth() ) {
        barWidth = ba.fixedBarWidth();
        groupWidth += barWidth;

        // Pending Michel set a min and max value for the groupWidth
        // related to the area.width
        if ( groupWidth < 0 )
            groupWidth = 0;

        if ( groupWidth  * rowCount > width )
            groupWidth = width / rowCount;
    }

    // maxLimit: allow the space between bars to be larger until area.width()
    // is covered by the groups.
    qreal maxLimit = rowCount * (groupWidth + ((colCount-1) * ba.fixedDataValueGap()) );


    //Pending Michel: FixMe
    if ( ba.useFixedDataValueGap() ) {
        if ( width > maxLimit )
            spaceBetweenBars += ba.fixedDataValueGap();
        else
            spaceBetweenBars = ((ctx->rectangle().width()/rowCount) - groupWidth)/(colCount-1);
    }

    if ( ba.useFixedValueBlockGap() )
        spaceBetweenGroups += ba.fixedValueBlockGap();

    calculateValueAndGapWidths( rowCount, colCount,groupWidth,
                                barWidth, spaceBetweenBars, spaceBetweenGroups );
    
    LabelPaintCache lpc;
    const qreal maxValue = 100.0; // always 100 %
    qreal sumValues = 0;
    QVector <qreal > sumValuesVector;

    //calculate sum of values for each column and store
    for ( int row = 0; row < rowCount; ++row )
    {
        for ( int col = 0; col < colCount; ++col )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, col );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            //if ( point.value > 0 )
            sumValues += qMax( point.value, -point.value );
            if ( col == colCount - 1 ) {
                sumValuesVector <<  sumValues ;
                sumValues = 0;
            }
        }
    }

    // calculate stacked percent value
    for ( int curRow = rowCount - 1; curRow >= 0; --curRow )
    {
        qreal offset = spaceBetweenGroups;
        if ( ba.useFixedBarWidth() )
            offset -= ba.fixedBarWidth();
        
        if ( offset < 0 )
            offset = 0;

        for ( int col = 0; col < colCount ; ++col )
        {
            qreal threeDOffset = 0.0;
            const CartesianDiagramDataCompressor::CachePosition position( curRow, col );
            const CartesianDiagramDataCompressor::DataPoint p = compressor().data( position );
            QModelIndex sourceIndex = attributesModel()->mapToSource( p.index );
            ThreeDBarAttributes threeDAttrs = diagram()->threeDBarAttributes( sourceIndex );

            if ( threeDAttrs.isEnabled() ) {
                if ( barWidth > 0 ) {
                    barWidth =  (width - ((offset+(threeDAttrs.depth()))*rowCount))/ rowCount;
                	threeDOffset = threeDAttrs.depth();
                }
                if ( barWidth <= 0 ) {
                    barWidth = 0.1;
                    threeDOffset = (width - (offset*rowCount))/ rowCount;
                }
            } else {
                barWidth = (width - (offset*rowCount))/ rowCount;
            }

            const qreal value = qMax( p.value, -p.value );
            qreal stackedValues = 0.0;
            qreal key = 0.0;
            
            // calculate stacked percent value
            // we only take in account positives values for now.
            for ( int k = col; k >= 0 ; --k )
            {
                const CartesianDiagramDataCompressor::CachePosition position( curRow, k );
                const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
                stackedValues += qMax( point.value, -point.value );
                key = point.key;
            }

            QPointF point, previousPoint;
            if ( sumValuesVector.at( curRow ) != 0 && value > 0 ) {
                QPointF dataPoint( ( stackedValues / sumValuesVector.at( curRow ) * maxValue ), key + 1 );
                point = ctx->coordinatePlane()->translate( dataPoint );
                point.ry() += offset / 2 + threeDOffset;

                previousPoint = ctx->coordinatePlane()->translate( QPointF( ( ( stackedValues - value) / sumValuesVector.at( curRow ) * maxValue ), key + 1 ) );
            }
            
            const qreal barHeight = point.x() - previousPoint.x();
            
            point.setX( point.x() - barHeight );

            const QRectF rect = QRectF( point, QSizeF( barHeight, barWidth ) ).translated( 1, 0 );
            m_private->addLabel( &lpc, sourceIndex, nullptr, PositionPoints( rect ), Position::North,
                                 Position::South, value );
            paintBars( ctx, sourceIndex, rect, maxDepth );
        }
    }
    m_private->paintDataValueTextsAndMarkers( ctx, lpc, false );
}
