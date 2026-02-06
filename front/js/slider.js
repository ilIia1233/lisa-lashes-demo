document.addEventListener("DOMContentLoaded", function () {
  console.log("DOM loaded, starting slider initialization...");
  
  const sliderWrapper = document.querySelector(".product-slider-wrapper");
  const sliderList = document.querySelector(".product-slider-list");
  const slides = document.querySelectorAll(".product-slider-card");
  const rightBtn = document.querySelector(".slider-arrow.right");
  const leftBtn = document.querySelector(".slider-arrow.left");
  
  console.log("Slider wrapper:", sliderWrapper);
  console.log("Slider list:", sliderList);
  console.log("Slides found:", slides.length);
  console.log("Right button:", rightBtn);
  console.log("Left button:", leftBtn);
  
  if (!sliderList || !slides.length || !rightBtn || !leftBtn) {
    console.error("Missing elements!");
    return;
  }
  
  let currentSlide = 0;
  const numberOfSlides = slides.length;
  
  console.log("Total slides:", numberOfSlides);
  
  // Test button clicks
  rightBtn.addEventListener("click", () => {
    console.log("Right button clicked! Current slide:", currentSlide);
    currentSlide++;
    if (currentSlide >= numberOfSlides) {
      currentSlide = 0;
    }
    console.log("New slide:", currentSlide);
    
    const slideWidth = slides[0].offsetWidth + 30;
    const offset = currentSlide * slideWidth;
    console.log("Slide width:", slideWidth, "Offset:", offset);
    
    sliderList.style.transform = `translateX(-${offset}px)`;
  });
  
  leftBtn.addEventListener("click", () => {
    console.log("Left button clicked! Current slide:", currentSlide);
    currentSlide--;
    if (currentSlide < 0) {
      currentSlide = numberOfSlides - 1;
    }
    console.log("New slide:", currentSlide);
    
    const slideWidth = slides[0].offsetWidth + 30;
    const offset = currentSlide * slideWidth;
    console.log("Slide width:", slideWidth, "Offset:", offset);
    
    sliderList.style.transform = `translateX(-${offset}px)`;
  });
});
