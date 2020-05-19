#include "MemeField.h"
#include <assert.h>
#include <random>
#include "SpriteCodex.h"


void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2 & screenPos, Graphics & gfx) const
{
	switch (state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;
	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;
	case State::Revealed:
		if (!hasMeme)
		{
			SpriteCodex::DrawTile0(screenPos, gfx);
		}
		else
		{
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		break;
	}
}

void MemeField::Tile::Reveal()
{
	assert(state == State::Hidden);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width * height);

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawn = 0; nSpawn < nMemes; nSpawn++)
	{
		Vei2 spawnPos;

		do 
		{
			spawnPos = { xDist(rng), yDist(rng) };
		}
		while (TileAt(spawnPos).HasMeme());

		TileAt(spawnPos).SpawnMeme();
	}
}

void MemeField::Draw(Graphics & gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++)
	{
		while (gridPos.x < width)
		{
			TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize, gfx);
			gridPos.x++;
		}
		gridPos.x = 0;
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0,width* SpriteCodex::tileSize,0,height* SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2 screenPos)
{
	const Vei2 gridPos = ScreenToGrid(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed())
	{
		tile.Reveal();
	}
}

MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos)
{
	return field[gridPos.x + gridPos.y * width];
}

const MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos) const
{
	return field[gridPos.x + gridPos.y * width];
}

Vei2 MemeField::ScreenToGrid(const Vei2 & screenPos) const
{
	return screenPos / SpriteCodex::tileSize;
}
