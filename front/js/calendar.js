const daysEl = document.getElementById("days");
const monthYearEl = document.getElementById("monthYear");
const prevBtn = document.getElementById("prev");
const nextBtn = document.getElementById("next");
const todayBtn = document.getElementById("todayBtn");

let date = new Date();

const months = [
  "January","February","March","April","May","June",
  "July","August","September","October","November","December"
];

function renderCalendar() {
  date.setDate(1);

  const month = date.getMonth();
  const year = date.getFullYear();

  const firstDay = date.getDay();
  const lastDay = new Date(year, month + 1, 0).getDate();
  const prevLastDay = new Date(year, month, 0).getDate();

  monthYearEl.innerText = `${months[month]} ${year}`;
  daysEl.innerHTML = "";

  for (let x = firstDay; x > 0; x--) {
    daysEl.innerHTML += `<div class="day">${prevLastDay - x + 1}</div>`;
  }

  for (let i = 1; i <= lastDay; i++) {
    if (
      i === new Date().getDate() &&
      month === new Date().getMonth() &&
      year === new Date().getFullYear()
    ) {
      daysEl.innerHTML += `<div class="day current today">${i}</div>`;
    } else {
      daysEl.innerHTML += `<div class="day current">${i}</div>`;
    }
  }
}

prevBtn.onclick = () => {
  date.setMonth(date.getMonth() - 1);
  renderCalendar();
};

nextBtn.onclick = () => {
  date.setMonth(date.getMonth() + 1);
  renderCalendar();
};

todayBtn.onclick = () => {
  date = new Date();
  renderCalendar();
};

renderCalendar();
