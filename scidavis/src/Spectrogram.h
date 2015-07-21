/***************************************************************************
	File                 : Spectrogram.h
	Project              : SciDAVis
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : SciDAVis's Spectrogram Class
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "Matrix.h"
#include <qwt_raster_data.h>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>

#include <tr1/memory>

class MatrixData;

class Spectrogram: public QwtPlotSpectrogram
{
public:
  Spectrogram();
  // reference to \a m kept, so don't pass a stack variable
  Spectrogram(Matrix& m);

  enum ColorMapPolicy{GrayScale, Default, Custom};

  Spectrogram* copy();
  /// returns a weak reference to the matrix the data was passed in on. 
  Matrix *matrix(){return d_matrix;};

  int levels(){return (int)contourLevels().size() + 1;};
  void setLevelsNumber(int levels);

  bool hasColorScale();
  int colorScaleAxis(){return color_axis;};
  void showColorScale(int axis, bool on = true);

  int colorBarWidth();
  void setColorBarWidth(int width);

  void setGrayScale();
  void setDefaultColorMap();
  static QwtLinearColorMap& defaultColorMap();

  void setCustomColorMap(const std::tr1::shared_ptr<QwtLinearColorMap>& map);
  void updateData(Matrix& m);

  //! Used when saving a project file
  QString saveToString();

  ColorMapPolicy colorMapPolicy(){return color_map_policy;};

protected:
  //! Pointer to the source data matrix
  QString matrixName;

  //! Axis used to display the color scale
  int color_axis;

  //! Flags
  ColorMapPolicy color_map_policy;

  std::tr1::shared_ptr<QwtLinearColorMap> color_map;
  Matrix* d_matrix;
};


class MatrixData: public QwtRasterData
{
public:
  // reference to \a m is stored, so please don't pass a stack variable
    MatrixData(const Matrix& m):
	d_matrix(m)
    {
      initRaster(m.boundingRect(), QSize(n_cols, n_rows));
	n_rows = d_matrix.numRows();
	n_cols = d_matrix.numCols();

	d_m = new double* [n_rows];
	for ( int l = 0; l < n_rows; ++l)
		d_m[l] = new double [n_cols];

	for (int i = 0; i < n_rows; i++)
         for (int j = 0; j < n_cols; j++)
           d_m[i][j] = d_matrix.cell(i, j);

	m.range(&min_z, &max_z);

	x_start = d_matrix.xStart();
	dx = (d_matrix.xEnd() - x_start)/(double)n_cols;

	y_start = d_matrix.yStart();
	dy = (d_matrix.yEnd() - y_start)/(double)n_rows;
    }

	~MatrixData()
	{
	for (int i = 0; i < n_rows; i++)
		delete [] d_m[i];

	delete [] d_m;
	};

    virtual QwtRasterData *copy() const
    {
        return new MatrixData(d_matrix);
    }

    virtual QwtDoubleInterval range() const
    {
        return QwtDoubleInterval(min_z, max_z);
    }

	virtual QSize rasterHint (const QwtDoubleRect &) const
	{
		return QSize(n_cols, n_rows);
	}

    virtual double value(double x, double y) const;

private:
	//! Pointer to the source data matrix
  const Matrix& d_matrix;

	//! Vector used to store in memory the data from the source matrix window
	double** d_m;

	//! Data size
	int n_rows, n_cols;

	//! Min and max values in the source data matrix
	double min_z, max_z;

	//! Data resolution in x(columns) and y(rows)
	double dx, dy;

	//! X axis left value in the data matrix
	double x_start;

	//! Y axis bottom value in the data matrix
	double y_start;
};

#endif