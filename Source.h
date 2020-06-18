#pragma once

#include <string>

void Initialize(sf::RectangleShape  cardStacks[7], sf::RectangleShape  cardSlot[4]);

void DrawAcePiles(int i, sf::RenderWindow& window, sf::RectangleShape  slotBackgrounds[4]);

void DrawCardPiles(int i, sf::RenderWindow& window, sf::RectangleShape  pileBackgrounds[7]);

void DrawDrawnCards(int i, sf::RenderWindow& window);

void DrawAllCards(sf::RenderWindow& window, sf::RectangleShape  slotBackgrounds[4], sf::RectangleShape  pileBackgrounds[7]);

void HandlePollEvents(sf::RenderWindow& window, sf::Vector2f& translatedPos, sf::RectangleShape  cardSlot[4], sf::RectangleShape cardStacks[7]);

void LoadFromHistory();

void MouseClicked(sf::RenderWindow& window, sf::Vector2f translatedPos, sf::RectangleShape cardStacks[7]);

void MouseReleased(sf::RectangleShape  cardSlot[4], sf::Vector2f& translatedPos, sf::RectangleShape cardStacks[7]);

void ResetCardPosition();

void MoveToSlot(sf::RectangleShape  cardSlot[4], int i);

void CheckForVictory();

bool CheckIfDoubleClicked(struct Card* card, sf::Time timeSinceLastClick);

void AddToHistory();

void RemoveCardFromPreviousLocation(struct Card* cardToRemove);

void HandleCardGraphics(struct Card* card, sf::RenderWindow& window);

void DrawUIText(sf::RenderWindow& window, const std::string textString, sf::Vector2f textPos, int textSize, int drawDirection);