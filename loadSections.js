document.addEventListener("DOMContentLoaded", () => {
  const sections = {
    header: "sections/header.html",
    promo: "sections/promo.html",
    welcome: "sections/welcome.html",
    testimonials: "sections/testimonials.html",
    products: "sections/products.html",
    booking: "sections/booking.html",
    footer: "sections/footer.html"
  };

  Object.entries(sections).forEach(([id, file]) => {
    const container = document.getElementById(id);
    if (!container) return;

    fetch(file)
      .then(response => {
        if (!response.ok) {
          throw new Error(`Failed to load ${file}`);
        }
        return response.text();
      })
      .then(html => {
        container.innerHTML = html;
      })
      .catch(err => {
        console.error(err);
      });
  });
});
