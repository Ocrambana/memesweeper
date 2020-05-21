#include "MemeField.h"
#include <assert.h>
#include <random>


void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2 & screenPos, FieldState fieldState, Graphics & gfx) const
{
	if (fieldState != FieldState::Lose)
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
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes ,gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
	else 
	{
		switch (state)
		{
		case State::Hidden:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;
		case State::Flagged:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (!hasMeme)
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;
		}
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

void MemeField::Tile::ToggleFlag()
{
	assert(!IsRevealed());

	if (state == State::Hidden)
	{
		state = State::Flagged;
	}
	else if (state == State::Flagged)
	{
		state = State::Hidden;
	}
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

void MemeField::Tile::SetNeighborMemeCount(int memeCount)
{
	assert(nNeighborMemes == -1);
	nNeighborMemes = memeCount;
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

	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++)
	{
		while (gridPos.x < width)
		{
			TileAt(gridPos).SetNeighborMemeCount(CountNeighborMemes(gridPos));
			gridPos.x++;
		}
		gridPos.x = 0;
	}
}

void MemeField::Draw(Graphics & gfx) const
{
	RectI rect = GetRect();
	gfx.DrawRect(rect.GetExpanded(borderWidth), borderColor);
	gfx.DrawRect(rect, SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++)
	{
		while (gridPos.x < width)
		{
			TileAt(gridPos).Draw(offset + gridPos * SpriteCodex::tileSize, state, gfx);
			gridPos.x++;
		}
		gridPos.x = 0;
	}
}

RectI MemeField::GetRect() const
{
	return RectI(offset.x, offset.x + width* SpriteCodex::tileSize, offset.y, offset.y + height* SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2 screenPos)
{
	if (!isMemed && state == FieldState::Memeing)
	{
		const Vei2 gridPos = ScreenToGrid(screenPos - offset);
		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed() && !tile.IsFlagged())
		{
			tile.Reveal();
			if (tile.HasMeme())
			{
				state = FieldState::Lose;
			}
		}
	}
}

void MemeField::OnFlagClick(const Vei2 screenPos)
{
	if (!isMemed && state == FieldState::Memeing)
	{
		const Vei2 gridPos = ScreenToGrid(screenPos - offset);
		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.ToggleFlag();
			
			if (IsGameWon())
			{
				state = FieldState::Win;
			}
		}
	}
}

bool MemeField::IsGameWon() const
{
	for (const Tile tile : field)
	{
		if ((tile.HasMeme() ^ tile.IsFlagged()))
		{
			return false;
		}
	}

	return true;
}

MemeField::FieldState MemeField::GetFieldState() const
{
	return state;
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

int MemeField::CountNeighborMemes(const Vei2 & gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int xEnd = std::min(width - 1, gridPos.x + 1);
	const int yEnd = std::min(height - 1, gridPos.y + 1);

	int count = 0;
	for (Vei2 gridPos = {xStart, yStart}; gridPos.y <= yEnd; gridPos.y++)
	{
		for ( gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
		{
			if (TileAt(gridPos).HasMeme())
			{
				count++;
			}
		}
	}

	return count;
}
