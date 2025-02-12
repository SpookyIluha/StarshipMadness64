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

#include "Blobs.h"

using namespace ceres;

Blob::Blob() {

	index = - 1;
	rCount = 0.0;
	gCount = 0.0;
	bCount = 0.0;
	colorR = 0.0;
	colorG = 0.0;
	colorB = 0.0;
	hue = 0.0;
	x0 = 0;
	y0 = 0;
	x1 = 0;
	y1 = 0;
	xC = 0.0;
	yC = 0.0;
	numPixels = 0;
	nextBrother = - 1;
	firstChild = - 1;
	parent = - 1;
	valid = false;
	mark = - 1;

}

Blob::~Blob() {
}

Blobs::Blobs() {

	resX = 0;
	resY = 0;
	maxBlobs = maxBlobs;
	blobs = NULL;
	blobsImage = NULL;
	numBlobs = 0;
	rootBlobIndex = -1;

}

Blobs::~Blobs() {
}

bool Blobs::init( int32_t resX, int32_t resY, int32_t maxBlobs, std::string &error ) {

	if ( resX <= 0 || resY <= 0 ) {
		error = "Invalid blobs image width or height.";
		return false;
	}

	if ( maxBlobs <= 0 ) {
		error = "Invalid specified max number of blobs.";
		return false;
	}

	this->resX = resX;
	this->resY = resY;
	this->maxBlobs = maxBlobs;

	blobs = new Blob[ maxBlobs ];
	if ( ! blobs ) {
		error = "Not enough memory to allocate the blobs array.";
		return false;
	}

	for ( int32_t i = 0; i < maxBlobs; i ++ ) blobs[ i ].index = i;

	blobsImage = new int32_t[ resX * resY ];

	resetBlobs();

	return true;

}

void Blobs::resetBlobs() {

    for ( int32_t i = 0; i < maxBlobs; i ++ ) {

		Blob *b = &blobs[ i ];
		b->valid = false;
		b->adjacents.clear();

	}

    numBlobs = 0;
    rootBlobIndex = -1;

    fillBlobsImage( - 1 );

}

void Blobs::fillBlobsImage( int32_t value ) {

	int32_t numPixels = resX * resY;
	for ( int32_t i = 0; i < numPixels; i ++ ) blobsImage[ i ] = value;

}

int32_t Blobs::getBlobNumberOfBrothers( Blob *b ) {

	int32_t n = 0;
	while ( b->nextBrother != -1 ) {
		b = &blobs[ b->nextBrother ];
		n ++;
	}

	return n;
}

int32_t Blobs::getBlobNumberOfChilds( Blob *b ) {

	if ( b->firstChild == -1 ) return 0;

	return getBlobNumberOfBrothers( &blobs[ b->firstChild ] ) + 1;
}

bool Blobs::doBlobsIntersect( Blob *b1, Blob *b2 ) {
	return ( b1->x0 <= b2->x1  ) && ( b1->x1 >= b2->x0 ) &&
           ( b1->y0 <= b2->y1  ) && ( b1->y1 >= b2->y0 );
}

bool Blobs::paintBlobsInImage( Image *image, int32_t filterBlobIndex ) {

	if ( image->numChannels != 3 ) return false;

	int32_t p = 0;
	int32_t p2 = 0;
	float *data = image->data;
	for ( int32_t j = 0; j < resY; j++ ) {
		for ( int32_t i = 0; i < resX; i++ ) {


			int32_t index = blobsImage[ p ];

			if ( filterBlobIndex >= 0 && filterBlobIndex != index ) index = - 1;

			if ( index >= 0 ) {

				Blob *b = &blobs[ index ];

				data[ p2 ] = b->colorR;
				data[ p2 + 1 ] = b->colorG;
				data[ p2 + 2 ] = b->colorB;

			}

			p ++;
			p2 += 3;
		}
	}

	return true;

}

bool Blobs::drawBlobRectangle( Image *image, Blob *b, float rgb[ 3 ] ) {

	if ( image->numChannels != 3 ) return false;

	for ( int32_t i = b->x0, j = b->y0; i <= b->x1; i ++ ) image->setRGB( i, j, rgb );
	for ( int32_t i = b->x0, j = b->y0; j <= b->y1; j ++ ) image->setRGB( i, j, rgb );
	for ( int32_t i = b->x0, j = b->y1; i <= b->x1; i ++ ) image->setRGB( i, j, rgb );
	for ( int32_t i = b->x1, j = b->y0; j <= b->y1; j ++ ) image->setRGB( i, j, rgb );

	return true;
}

bool Blobs::analizeBlobs( Image *segmentationImage, Image *colorsImage, std::string &error  ) {

	/*
		segmentation image: An xRes * yRes image of 1 channel (float). Blobs are different if pixels
		in this image are different.

		colors image: An xRes * yRes image of 3 channels (3 * float). Blobs take their colors from the
		RGB values in this image. This parameter can be NULL (colors will not be saved in the blobs)

	*/

	if ( segmentationImage->numChannels != 1 ) {
		error = "Segmentation image must have one channel.";
		return false;
	}

	if ( colorsImage && colorsImage->numChannels != 3 ) {
		error = "Colors image must have 3 channels.";
		return false;
	}

	if ( segmentationImage->width != resX || segmentationImage->height != resY ) {
		error = "Segmentation image width and height don't correspond to blobs resX and resY.";
		return false;
	}

	if ( colorsImage && ( colorsImage->width != resX || colorsImage->height != resY ) ) {
		error = "Colors image width and height don't correspond to blobs resX and resY.";
		return false;
	}

	resetBlobs();

	float *segmImageData = segmentationImage->data;
	float *colImageData = colorsImage ? colorsImage->data : NULL;

	// Current pixel
	int32_t p = 0;

	// North pixel
	int32_t p0 = - resX;

	// West pixel
	int32_t p1 = -1;

	for ( int32_t j = 0; j < resY; j ++ ) {

		for ( int32_t i = 0; i < resX; i ++ ) {

			// If current pixel is compatible with west pixel,
			if ( i > 0 && segmImageData[ p1 ] == segmImageData[ p ] ) {

				// Add current pixel to the west blob
				int32_t westBlobIndex = blobsImage[ p1 ];
				blobsImage[ p ] = westBlobIndex;
				Blob *westBlob = &blobs[ westBlobIndex ];
				westBlob->numPixels ++;

				if ( colImageData ) {
					int32_t p3 = p * 3;
					westBlob->rCount += colImageData[ p3 + 0 ];
					westBlob->gCount += colImageData[ p3 + 1 ];
					westBlob->bCount += colImageData[ p3 + 2 ];
				} else westBlob->rCount += segmImageData[ p ];

				if ( westBlob->x0 > i ) {
					westBlob->x0 = i;
				}
				if ( westBlob->y0 > j ) {
					westBlob->y0 = j;
				}
				if ( westBlob->x1 < i ) {
					westBlob->x1 = i;
				}
				if ( westBlob->y1 < j ) {
					westBlob->y1 = j;
				}

				if ( j > 0 ) {

					// If north blob is not compatible, make them adjacent
					int32_t northBlobIndex = blobsImage[ p0 ];
					if ( segmImageData[ p ] != segmImageData[ p0 ] ) {
						makeBlobsAdjacent( blobsImage[ p ], northBlobIndex );
					}
					else {

						// If they are compatible but they are currently different blobs, merge them.
						if ( westBlobIndex != northBlobIndex ) {

							Blob *northBlob = &blobs[ northBlobIndex ];

							// Blob with higher id is added to the one with lowest id (resp. source and dest blobs)
							int32_t destBlobIndex = westBlobIndex;
							int32_t srcBlobIndex = northBlobIndex;
							Blob *destBlob = westBlob;
							Blob *srcBlob = northBlob;
							if ( westBlobIndex > northBlobIndex ) {
								destBlobIndex = northBlobIndex;
								srcBlobIndex = westBlobIndex;
								destBlob = northBlob;
								srcBlob = westBlob;
							}

							destBlob->numPixels += srcBlob->numPixels;
							destBlob->rCount += srcBlob->rCount;
							destBlob->gCount += srcBlob->gCount;
							destBlob->bCount += srcBlob->bCount;

							if ( destBlob->x0 > srcBlob->x0 ) destBlob->x0 = srcBlob->x0;
							if ( destBlob->y0 > srcBlob->y0 ) destBlob->y0 = srcBlob->y0;
							if ( destBlob->x1 < srcBlob->x1 ) destBlob->x1 = srcBlob->x1;
							if ( destBlob->y1 < srcBlob->y1 ) destBlob->y1 = srcBlob->y1;

							// Removes the source blob from blobs adjacent to it and
							// joins adjacency lists from both blobs
							joinAdjacents( srcBlob, destBlob );

							// Converts all pixels from source blob to dest blob
							int32_t oldP = resX * srcBlob->y0 + srcBlob->x0;
							int32_t dbfx = resX - ( srcBlob->x1 - srcBlob->x0 + 1 );
							for ( int32_t jbf = srcBlob->y0; jbf <= srcBlob->y1; jbf ++ ) {
								for ( int32_t ibf = srcBlob->x0; ibf <= srcBlob->x1; ibf ++ ) {

									if ( blobsImage[ oldP ] == srcBlobIndex ) {
										blobsImage[ oldP ] = destBlobIndex;
									}

									oldP ++;
								}
								oldP += dbfx;
							}

							// Desaparece el blob fuente
							srcBlob->valid = false;
							numBlobs --;
						}
					}
				}

			} else {

				// If current pixel is compatible with north pixel,
				if ( j > 0 && segmImageData[ p ] == segmImageData[ p0 ] ) {

					// Add current pixel to north blob
					int32_t blobIndex = blobsImage[ p0 ];
					blobsImage[ p ] = blobIndex;

					Blob *b = &blobs[ blobIndex ];
					b->numPixels ++;

					if ( colImageData ) {
						int32_t p3 = p * 3;
						b->rCount += colImageData[ p3 + 0 ];
						b->gCount += colImageData[ p3 + 1 ];
						b->bCount += colImageData[ p3 + 2 ];
					} else b->rCount += segmImageData[ p ];

					if ( b->x0 > i ) b->x0 = i;
					if ( b->y0 > j ) b->y0 = j;
					if ( b->x1 < i ) b->x1 = i;
					if ( b->y1 < j ) b->y1 = j;

				}
				else {

					// A new blob of 1 pixel is created

					if ( numBlobs == maxBlobs ) {
						// Not enough blob buckets
						resetBlobs();
						error = "Not enough blob buckets. Please increment maxBlobs.";
						return false;
					}

					// Find first not used blob
					int32_t blobIndex = 0;
					while ( blobIndex < maxBlobs && blobs[ blobIndex ].valid ) blobIndex ++;

					if ( blobIndex >= maxBlobs ) {
						resetBlobs();
						error = "Not enough blob buckets. Please increment maxBlobs (internal error)";
						return false;
					}

					Blob *b = &blobs[ blobIndex ];

					b->valid = true;
					numBlobs ++;
					b->numPixels = 1;
					blobsImage[ p ] = blobIndex;

					b->hue = segmImageData[ p ];

					if ( colImageData ) {
						int32_t p3 = p * 3;
						b->rCount = colImageData[ p3 + 0 ];
						b->gCount = colImageData[ p3 + 1 ];
						b->bCount = colImageData[ p3 + 2 ];
					} else {
						b->rCount = segmImageData[ p ];
						b->gCount = 0;
						b->bCount = 0;
					}

					b->x0 = i;
					b->x1 = i;
					b->y0 = j;
					b->y1 = j;

					// Clears adjacents
					b->adjacents.clear();

					// If there is north neighbour it is different and we make it adjacent
					if ( j > 0 ) makeBlobsAdjacent( blobIndex, blobsImage[ p0 ] );

				}

				// If there is west neighbour it is different and we make it adjacent
				if ( i > 0 ) makeBlobsAdjacent( blobsImage[ p ] , blobsImage[ p1 ] );
			}

            p ++;
            p0 ++;
            p1 ++;

		}

	}


	for ( int32_t i = 0; i < numBlobs; i++ ) {

		Blob *b = &blobs[ i ];

		if ( colImageData ) {

			b->colorR = b->rCount / b->numPixels;
			b->colorG = b->gCount / b->numPixels;
			b->colorB = b->bCount / b->numPixels;

		} else {

			b->colorR = b->rCount / b->numPixels;
			b->colorG = b->colorR;
			b->colorB = b->colorR;

		}

		b->xC = ( b->x0 + b->x1 ) * 0.5;
		b->yC = ( b->y0 + b->y1 ) * 0.5;


		b->nextBrother = - 1;
		b->firstChild = - 1;
		b->parent = - 1;
		b->mark = NOT_MARKED;

	}

	return true;

}

void Blobs::makeBlobsAdjacent( int32_t blob0Index, int32_t blob1Index ) {

	if ( blob0Index == blob1Index ) return;

	Blob *b0 = &blobs[ blob0Index ];
	Blob *b1 = &blobs[ blob1Index ];

	uint32_t i = 0;
	while ( i < b0->adjacents.size() && b0->adjacents[ i ] != b1 ) i ++;

	// If they were not adjacent, make them so
	if ( i >= b0->adjacents.size() ) {
		b0->adjacents.push_back( b1 );
		b1->adjacents.push_back( b0 );
	}

}

void Blobs::joinAdjacents( Blob *srcBlob, Blob *destBLob ) {

	// Removes srcBlob from the adjacency lists of its adjacent blobs and then adds
	// the adjacent blobs of srcBlob to destBlob if they weren0t already adjacent to destBLob.

	for ( uint32_t i = 0; i < srcBlob->adjacents.size(); i ++ ) {

		Blob *bw = srcBlob->adjacents[ i ];

		uint32_t ibSrc = 0;
		while ( ibSrc < bw->adjacents.size() && bw->adjacents[ ibSrc ] != srcBlob ) ibSrc ++;

		// Removes srcBlob from adjacent list of bw
		bw->adjacents.erase( bw->adjacents.begin() + ibSrc );

		makeBlobsAdjacent( bw->index, destBLob->index );

	}

}

bool Blobs::blobContainsBlob( Blob *a, Blob *b ) {

	// Returns true iif 'a' rectangle contains 'b' rectangle with a margin of 1 pixel
	return ( a->x0 < b->x0 ) && ( a->y0 < b->y0 ) && ( a->x1 > b->x1 ) && ( a->y1 > b->y1 );

}

void Blobs::makeParent( Blob *x, Blob *y ) {

	// Makes x the parent of y

    y->parent = x->index;

    if ( x->firstChild == - 1 ) x->firstChild = y->index;
    else {
        int32_t indC = x->firstChild;
        Blob *c = &blobs[ indC ];
        while ( c->nextBrother != - 1 ) {
            indC = c->nextBrother;
            c = &blobs[ indC ];
        }
        c->nextBrother= y->index;
    }
    y->nextBrother = - 1;

}

bool Blobs::analizeHierarchy( std::string &error ) {

	// Must have called analizeBlobs method first.

	// Loop all current valid blobs
	for ( int32_t i = 0; i < numBlobs; i++ ) {

		Blob *blobY = &blobs[ i ];

		if ( ! blobY->valid ) continue;

		// If parent has been assigned by expansion, continue
		if ( blobY->parent != -1 ) continue;

		// If it is a blob that touches the border of the image, continue
		if (
			blobY->x0 == 0 ||
			blobY->y0 == 0 ||
			blobY->x1 == resX - 1 ||
			blobY->y1 == resY - 1 ) continue;

		// Loop all adjacents of blobY
		int32_t nAdyY = blobY->adjacents.size();
		for ( int32_t j = 0; j < nAdyY; j++ ) {

			Blob *blobX = blobY->adjacents[ j ];

			// If X doesn't contain Y with border of 1 pixel, it is not its parent.
			if ( ! blobContainsBlob( blobX, blobY ) ) continue;


			bool yComesOutOfX = false;
			candidateAdjacents.clear();

			// Loop all blobC, common adjacents between X and Y
			int32_t nAdyX = blobX->adjacents.size();
			for ( int32_t k = 0; k < nAdyY; k++ ) {
				Blob *blobC = blobY->adjacents[ k ];
				for ( int32_t l = 0; l < nAdyX; l++ ) {
					if ( blobC == blobX->adjacents[ l ] ) {
						if ( ! blobContainsBlob( blobX, blobC ) ) {
							yComesOutOfX = true;
							break;
						}
						blobC->mark = MARKED;
						candidateAdjacents.push_back( blobC );
					}
				}

				if ( yComesOutOfX ) break;

			}

			uint32_t numExpanded = 0;
			while ( ( ! yComesOutOfX ) && ( numExpanded < candidateAdjacents.size() ) ) {

				for ( uint32_t k = 0; k < candidateAdjacents.size(); k ++ ) {

					Blob *blobC = candidateAdjacents[ k ];

					if ( blobC->mark ==  MARKED ) {

						int32_t nAdyC = blobC->adjacents.size();
						for ( int32_t l = 0; l < nAdyC; l++ ) {
							Blob *blobV = blobC->adjacents[ l ];
							if ( blobV->mark == NOT_MARKED ) {
								if ( ! blobContainsBlob( blobX, blobV ) ) {
									yComesOutOfX = true;
									break;
								}
								else {
									candidateAdjacents.push_back( blobV );
									blobV->mark = MARKED;
								}
							}
						}

						if ( yComesOutOfX ) break;

						blobC->mark = EXPANDED;
						numExpanded ++;

					}
				}
			}

			// Desmarca todos los blobs
			int32_t numAdjCandidates = candidateAdjacents.size();
			for ( int32_t k = 0; k < numAdjCandidates; k++ ) {
				Blob *blobC = candidateAdjacents[ k ];
				blobC->mark = NOT_MARKED;
			}

			if ( ! yComesOutOfX ) {

				// X is parent of Y
				makeParent( blobX, blobY );

				// The list of candidateAdjacents are all sons of X
				for ( int32_t k = 0; k < numAdjCandidates; k ++ ) {
					makeParent( blobX, candidateAdjacents[ k ] );
				}

				break;
			}
		}
	}

	// Finds all root nodes and sets the first one as main root node and the rest as its brothers
	int32_t i = 0;
	while ( i < numBlobs ) {

		rootBlobIndex = i;
		Blob *rootBlob = &blobs[ i ];
		if ( rootBlob->valid && rootBlob->parent == - 1 ) break;
		i++;

	}

	if ( i < numBlobs ) {

		Blob *blobX = &blobs[ rootBlobIndex ];
		for ( int32_t indY = rootBlobIndex + 1; indY < numBlobs; indY++ ) {
			Blob *blobY = &blobs[ indY ];
			if ( blobY->valid && blobY->parent == - 1 ) {
				blobX->nextBrother = indY;
				blobX = blobY;
			}
		}
		blobX->nextBrother = - 1;
	}
	else {
		// This should never happen
		rootBlobIndex = -1;
		error = "Internal error, root node is invalid.";
		return false;
	}

	return true;

}
