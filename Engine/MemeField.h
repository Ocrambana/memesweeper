#pragma once

#include "Vei2.h" 
#include "Graphics.h"
#include "RectI.h"
#include "SpriteCodex.h"

class MemeField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMeme();
		bool HasMeme() const;
		void Draw(const Vei2& screenPos, bool memed, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void SetNeighborMemeCount(int memeCount);
	
	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int nNeighborMemes = -1;
	};

public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2 screenPos);
	void OnFlagClick(const Vei2 screenPos);

private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& screenPos) const;
	int CountNeighborMemes(const Vei2& gridPos);

private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	static constexpr int borderWidth = 5;
	static constexpr Color borderColor = Colors::Cyan;
	const Vei2 offset = Vei2((Graphics::ScreenWidth - width * SpriteCodex::tileSize) / 2  , (Graphics::ScreenHeight - height * SpriteCodex::tileSize) / 2);
	bool isMemed = false;
	Tile field[width * height];
};

