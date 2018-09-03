#include "synfont.h"

SynFont::SynFont()
{
	SAFE_INIT(m_Font);
	SAFE_INIT(m_Texture);
}

SynFont::SynFont(const SynFont& other)
{

}

SynFont::~SynFont()
{

}

bool SynFont::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the text file containing the font data.
	result = LoadFontData(fontFilename);
	SAFE_CHECKEXIST(result);

	// Load the texture that has the font characters on it.
	result = LoadTexture(device, textureFilename);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynFont::Shutdown()
{
	// Release the font texture.
	ReleaseTexture();

	// Release the font data.
	ReleaseFontData();

	return;
}

bool SynFont::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	m_Font = new FontType[95];
	SAFE_CHECKEXIST(m_Font);

	// Read in the font size and spacing between chars.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (i = 0; i<95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}

void SynFont::ReleaseFontData()
{
	// Release the font data array.
	if (m_Font)
	{
		SAFE_DELETE_ARRAY(m_Font);
	}

	return;
}

bool SynFont::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new SynTexture;
	SAFE_CHECKEXIST(m_Texture);

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynFont::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		SAFE_DELETE(m_Texture);
	}

	return;
}

ID3D11ShaderResourceView* SynFont::GetTexture()
{
	return m_Texture->GetTexture();
}

void SynFont::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for (i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}