#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"
#include "rgbimage.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2) : Size(10, 1, 10)
{
	if (HeightMap && DetailMap1 && DetailMap2)
	{
		bool loaded = load(HeightMap, DetailMap1, DetailMap2, nullptr);
		if (!loaded)
			throw std::exception();

	}
}

Terrain::~Terrain()
{

}

bool Terrain::load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixTex)
{
	if (!HeightTex.load(HeightMap))
		return false;
	if (!DetailTex[0].load(DetailMap1))
		return false;
	if (!DetailTex[1].load(DetailMap2))
		return false;
	if (!this->MixTex.load(MixTex))
		return false;

	const RGBImage* rgbImage = HeightTex.getRGBImage();
	RGBImage dst(rgbImage->width(),rgbImage->height());
	RGBImage::SobelFilter(dst, *rgbImage, 10);
	dst.saveToDisk("mixmap.bmp");

	

	VB.begin();
	IB.begin();
	for (int z = 0; z < rgbImage->height(); z++) {
		for (int x = 0; x < rgbImage->width(); x++) {
			Vector normale;
			Vector vertizes[6];
			Vector vertex;
			vertex = Vector(this->width() / rgbImage->width() * x - this->width() / 2, rgbImage->getPixelColor(x, z).R * this->height(), this->depth() / rgbImage->height() * z - this->depth() / 2);
			if (x > 0) {
				vertizes[0] = Vector(this->width() / rgbImage->width() * (x - 1) - this->width() / 2, rgbImage->getPixelColor(x - 1, z).R * this->height(), this->depth() / rgbImage->height() * z - this->depth() / 2);
				if (z < rgbImage->height() - 1) {
					vertizes[1] = Vector(this->width() / rgbImage->width() * (x - 1) - this->width() / 2, rgbImage->getPixelColor(x - 1, z + 1).R * this->height(), this->depth() / rgbImage->height() * (z + 1) - this->depth() / 2);
				}
			}
			if (z < rgbImage->height() - 1) {
				vertizes[2] = Vector(this->width() / rgbImage->width() * (x)-this->width() / 2, rgbImage->getPixelColor(x, z + 1).R * this->height(), this->depth() / rgbImage->height() * (z + 1) - this->depth() / 2);
			}
			if (x < rgbImage->width() - 1) {
				vertizes[3] = Vector(this->width() / rgbImage->width() * (x + 1) - this->width() / 2, rgbImage->getPixelColor(x + 1, z).R * this->height(), this->depth() / rgbImage->height() * (z)-this->depth() / 2);
				if (z > 0) {
					vertizes[4] = Vector(this->width() / rgbImage->width() * (x + 1) - this->width() / 2, rgbImage->getPixelColor(x + 1, z - 1).R * this->height(), this->depth() / rgbImage->height() * (z - 1) - this->depth() / 2);
				}
			}
			if (z > 0) {
				vertizes[5] = Vector(this->width() / rgbImage->width() * (x)-this->width() / 2, rgbImage->getPixelColor(x, z - 1).R * this->height(), this->depth() / rgbImage->height() * (z - 1) - this->depth() / 2);
			}
			if (x > 0 && z < rgbImage->height()) {
				normale = normale + (vertizes[0] - vertex).cross(vertizes[1] - vertex); // W x NW
				normale = normale + (vertizes[1] - vertex).cross(vertizes[2] - vertex); // NW x N
			}
			if (x < rgbImage->width() && z < rgbImage->height()) {
				normale = normale +  (vertizes[2] - vertex).cross(vertizes[3] - vertex); // N x E
			}
			if (x < rgbImage->width() && z > 0) {
				normale = normale +  (vertizes[3] - vertex).cross(vertizes[4] - vertex); // E x SE
				normale = normale +  (vertizes[4] - vertex).cross(vertizes[5] - vertex); // SE x S
			}
			if (x > 0 && z > 0) {
				normale = normale + (vertizes[5] - vertex).cross(vertizes[0] - vertex); // S x W
			}
			//VB.addTexcoord0(0, 0);
			this->VB.addTexcoord0(x / (float) rgbImage->width() - 1, z / (float) rgbImage->height() - 1);
			this->VB.addTexcoord1(x / ((float) rgbImage->width() - 1) * 100, z / ((float) rgbImage->height() - 1) * 100);

			VB.addNormal(normale.normalize());
			VB.addVertex(vertex);
			if (z < rgbImage->height() && x < rgbImage->width()) {
				IB.addIndex(x + z * rgbImage->width());
				IB.addIndex(x + (z + 1) * rgbImage->width());
				IB.addIndex(x + 1 + z * rgbImage->width());
			}
			if (x > 0 && z < rgbImage->height()) {
				IB.addIndex(x + z * rgbImage->width());
				IB.addIndex(x - 1 + (z + 1) * rgbImage->width());
				IB.addIndex(x + (z + 1) * rgbImage->width());
			}
		}
	}
	VB.end();
	IB.end();
	return true;
}

void Terrain::shader(BaseShader* shader, bool deleteOnDestruction)
{
	BaseModel::shader(shader, deleteOnDestruction);
}

void Terrain::draw(const BaseCamera& Cam)
{
	applyShaderParameter();
	BaseModel::draw(Cam);

	VB.activate();
	IB.activate();
	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
	IB.deactivate();
	VB.deactivate();

}

void Terrain::applyShaderParameter()
{
	TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
	if (!Shader)
		return;

	Shader->mixTex(&MixTex);
	for (int i = 0; i < 2; i++)
		Shader->detailTex(i, &DetailTex[i]);
	Shader->scaling(Size);

	// TODO: add additional parameters if needed..
}

