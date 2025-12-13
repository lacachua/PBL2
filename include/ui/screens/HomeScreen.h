#pragma once

#include "UI/screens/BaseScreen.h"

#include "core/AppState.h"

#include "repositories/MovieRepository.h"

#include "UI/components/PosterSlider/PosterSlider.h"

#include <memory>

class HomeScreen : public BaseScreen {
private:
    RenderWindow& win;
    std::unique_ptr<PosterSlider> slider;
    std::unique_ptr<MovieRepository> repo;
    Clock clock;

    bool isLoggedIn = false;
    std::string currentUser;

public:
    HomeScreen(Font&, RenderWindow&);
    void update(Vector2f, bool, AppState&) override;
    void handleEvent(Vector2f, bool, AppState&, const Event* event = nullptr);
    void draw(RenderWindow&) override;
    void drawHeaderOnly(RenderWindow&);

    void setLoggedUser(const std::string&);

    MovieRepository* getRepository() const;
    int getSelectedMovieIndex() const;
    void clearSelectedMovieIndex();
};
