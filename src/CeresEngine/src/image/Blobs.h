/*
 * Copyright © 2020 - 2025 yomboprime https://codeberg.org/yombo
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#pragma once

#include <inttypes.h>
#include <string>
#include <vector>

#include "image/Image.h"

namespace ceres {

struct Blob {

	Blob();
	virtual ~Blob();

	int32_t index;
	float rCount;
	float gCount;
	float bCount;
	float colorR;
	float colorG;
	float colorB;
	float hue;
	int32_t x0;
	int32_t y0;
	int32_t x1;
	int32_t y1;
	float xC;
	float yC;
	int32_t numPixels;
	int32_t nextBrother;
	int32_t firstChild;
	int32_t parent;
	std::vector<Blob *> adjacents;
	bool valid;
	int32_t mark;

};

class Blobs {

public:

	Blobs();
	virtual ~Blobs();

	bool init( int32_t resX, int32_t resY, int32_t maxBlobs, std::string &error );
	bool analizeBlobs( Image *segmentationImage, Image *colorsImage, std::string &error );
	bool analizeHierarchy( std::string &error );

	int32_t getNumBlobs() { return numBlobs; }
	Blob *getBlob( int32_t index ) { return &blobs[ index ]; }
	int32_t getBlobNumberOfChilds( Blob *b );

	int32_t getRootBlobIndex() { return rootBlobIndex; }
	int32_t *getBlobsImage() { return blobsImage; }

	bool paintBlobsInImage( Image *image, int32_t filterBlobIndex = - 1 );
	bool drawBlobRectangle( Image *image, Blob *b, float rgb[ 3 ] );

private:

	void resetBlobs();
	void fillBlobsImage( int32_t value );
	int32_t getBlobNumberOfBrothers( Blob *b );
	bool doBlobsIntersect( Blob *b1, Blob *b2 );
	bool blobContainsBlob( Blob *a, Blob *b );

	void makeBlobsAdjacent( int32_t blob0Index, int32_t blob1Index );
	void joinAdjacents( Blob *srcBlob, Blob *destBLob );
	void makeParent( Blob *x, Blob *y );

	int32_t resX;
	int32_t resY;

	Blob *blobs;
	int32_t maxBlobs;

	int32_t numBlobs;

	int32_t *blobsImage;

	int32_t rootBlobIndex;

	const int32_t NOT_MARKED = 0;
	const int32_t MARKED = 1;
	const int32_t EXPANDED = 2;

	std::vector<Blob *> candidateAdjacents;

};

}
