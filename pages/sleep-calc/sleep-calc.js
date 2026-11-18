// pages/sleep-calc/sleep-calc.js
function pad(n) { return n < 10 ? '0' + n : '' + n; }

Page({
  data: {
    hourRange: [],
    minRange: [],
    wakeHour: 7,
    wakeMin: 30,
    wakeMinIdx: 6,
    wakeTimeText: '07:30',
    results: [],
    calculated: false
  },

  onLoad() {
    const hours = [];
    for (let i = 0; i < 24; i++) hours.push(i);
    const mins = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55];
    this.setData({
      hourRange: hours.map(h => String(h)),
      minRange: mins.map(m => String(m))
    });
  },

  onHourChange(e) {
    this.setData({ wakeHour: parseInt(e.detail.value) }, () => this.updateTimeText());
  },

  onMinChange(e) {
    const idx = parseInt(e.detail.value);
    this.setData({
      wakeMinIdx: idx,
      wakeMin: parseInt(this.data.minRange[idx])
    }, () => this.updateTimeText());
  },

  updateTimeText() {
    this.setData({
      wakeTimeText: pad(this.data.wakeHour) + ':' + pad(this.data.wakeMin)
    });
  },

  calcSleep() {
    const cycles = [5, 4, 3, 2];
    const wakeTotal = this.data.wakeHour * 60 + this.data.wakeMin;
    const results = [];

    for (let i = 0; i < cycles.length; i++) {
      const cyc = cycles[i];
      let sleepTotal = wakeTotal - cyc * 90;
      if (sleepTotal < 0) sleepTotal += 24 * 60;
      const sh = Math.floor(sleepTotal / 60);
      const sm = sleepTotal % 60;
      results.push({
        cyc: cyc,
        hours: (cyc * 1.5).toFixed(1),
        time: pad(sh) + ':' + pad(sm)
      });
    }

    this.setData({ results: results, calculated: true });
  }
});
