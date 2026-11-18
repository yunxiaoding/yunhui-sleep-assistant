// pages/sleep-timer/sleep-timer.js
const app = getApp();

function pad(n) { return n < 10 ? '0' + n : '' + n; }

function fmtTime(t) {
  const d = new Date(t);
  return pad(d.getHours()) + ':' + pad(d.getMinutes());
}

function fmtDate(t) {
  const d = new Date(t);
  return d.getFullYear() + '-' + pad(d.getMonth() + 1) + '-' + pad(d.getDate());
}

Page({
  data: {
    status: 'idle',        // idle | running | done
    startTime: 0,
    startStr: '',
    endStr: '',
    durationText: '',
    evalText: '',
    elapsedSec: 0
  },

  timer: null,

  startTimer() {
    const now = Date.now();
    this.setData({
      status: 'running',
      startTime: now,
      startStr: fmtTime(now),
      elapsedSec: 0
    });
    this.tick();
  },

  tick() {
    this.timer = setInterval(() => {
      const sec = Math.floor((Date.now() - this.data.startTime) / 1000);
      this.setData({ elapsedSec: sec });
    }, 1000);
  },

  stopTimer() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
    const startT = this.data.startTime;
    const endT = Date.now();
    const totalMin = Math.max(1, Math.floor((endT - startT) / 60000));

    let evalText;
    if (totalMin >= 420) evalText = '睡眠充足，状态很棒';
    else if (totalMin >= 300) evalText = '睡眠尚可，注意休息';
    else evalText = '睡眠不足，建议补觉';

    const hours = Math.floor(totalMin / 60);
    const minutes = totalMin % 60;

    // 保存记录到全局缓存
    app.addRecord({
      date: fmtDate(startT),
      start: fmtTime(startT),
      end: fmtTime(endT),
      minutes: totalMin,
      eval: evalText
    });

    this.setData({
      status: 'done',
      endStr: fmtTime(endT),
      durationText: hours + '小时' + minutes + '分钟',
      evalText: evalText
    });
  },

  resetTimer() {
    this.setData({
      status: 'idle',
      startTime: 0,
      startStr: '',
      endStr: '',
      durationText: '',
      evalText: '',
      elapsedSec: 0
    });
  },

  onUnload() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
  }
});
