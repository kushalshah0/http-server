let slideIndex = 0;

function showSlide(index) {
    const slides = document.getElementsByClassName("slides")[0];
    const totalSlides = document.getElementsByClassName("slide").length;
    
    // Calculate the new transform value
    const offset = -index * 100; // Each slide takes 100% of the width
    slides.style.transform = `translateX(${offset}%)`;

    // Show or hide buttons based on the current slide index
    const prevButton = document.querySelector('.prev');
    const nextButton = document.querySelector('.next');

    if (index === 0) {
        prevButton.style.display = "none"; // Hide previous button on the first slide
    } else {
        prevButton.style.display = "block"; // Show previous button on other slides
    }

    if (index === totalSlides - 1) {
        nextButton.style.display = "none"; // Hide next button on the last slide
    } else {
        nextButton.style.display = "block"; // Show next button on other slides
    }
}

function changeSlide(n) {
    const totalSlides = document.getElementsByClassName("slide").length;

    // Prevent going to the last slide from the first slide
    if (n === -1 && slideIndex === 0) {
        return; // Do nothing if trying to go back from the first slide
    }

    // Prevent going to the first slide from the last slide
    if (n === 1 && slideIndex === totalSlides - 1) {
        return; // Do nothing if trying to go forward from the last slide
    }

    slideIndex += n;

    // Wrap around the slide index
    if (slideIndex >= totalSlides) {
        slideIndex = totalSlides - 1; // Prevent going beyond the last slide
    }
    if (slideIndex < 0) {
        slideIndex = 0; // Prevent going before the first slide
    }
    
    showSlide(slideIndex);
}

// Initial display
showSlide(slideIndex);

// Keyboard navigation
document.addEventListener('keydown', function(event) {
    if (event.key === "ArrowRight") {
        changeSlide(1);
    } else if (event.key === "ArrowLeft") {
        changeSlide(-1);
    } else if (event.key === "f") { // Check for "F" key
        toggleFullScreen();
    }
});

// Fullscreen functionality
function toggleFullScreen() {
    const slider = document.querySelector('.slider');
    const fullscreenButton = document.querySelector('.fullscreen');

    if (!document.fullscreenElement) {
        slider.requestFullscreen().catch(err => {
            console.error(`Error attempting to enable full-screen mode: ${err.message} (${err.name})`);
        });
        fullscreenButton.style.display = "none"; // Hide the fullscreen button
    } else {
        document.exitFullscreen();
        fullscreenButton.style.display = "block"; // Show the fullscreen button
    }
}

// Listen for fullscreen change events
document.addEventListener('fullscreenchange', () => {
    const fullscreenButton = document.querySelector('.fullscreen');
    if (document.fullscreenElement) {
        fullscreenButton.style.display = "none"; // Hide the button when in fullscreen
    } else {
        fullscreenButton.style.display = "block"; // Show the button when exiting fullscreen
    }
});