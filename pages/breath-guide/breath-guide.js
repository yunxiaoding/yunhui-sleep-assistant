// pages/breath-guide/breath-guide.js
Page({
  data: {
    running: false,
    round: 0,
    phase: '',       // '吸气' | '屏息' | '呼气'
    phaseClass: '',  // inhale | hold | exhale
    countdown: 0,
    finished: false
  },

  timer: null,
  round: 0,
  phaseIdx: 0,
  countdown: 0,

  // [阶段名, 秒数, 样式类名]
  phases: [
    { name: '吸气', sec: 4, cls: 'inhale' },
    { name: '屏息', sec: 7, cls: 'hold' },
    { name: '呼气', sec: 8, cls: 'exhale' }
  ],

  startGuide() {
    this.round = 1;
    this.phaseIdx = 0;
    this.countdown = this.phases[0].sec;
    this.setData({
      running: true,
      finished: false,
      round: 1,
      phase: this.phases[0].name,
      phaseClass: this.phases[0].cls,
      countdown: this.phases[0].sec
    });
    this.runPhase();
  },

  runPhase() {
    this.timer = setInterval(() => {
      this.countdown--;
      if (this.countdown <= 0) {
        clearInterval(this.timer);
        this.timer = null;
        this.nextPhase();
      } else {
        this.setData({ countdown: this.countdown });
      }
    }, 1000);
  },

  nextPhase() {
    this.phaseIdx++;
    if (this.phaseIdx >= this.phases.length) {
      // 一轮完成
      this.phaseIdx = 0;
      this.round++;
      if (this.round > 4) {
        // 4轮完成，结束
        this.setData({ running: false, finished: true });
        return;
      }
    }
    const phase = this.phases[this.phaseIdx];
    this.countdown = phase.sec;
    this.setData({
      round: this.round,
      phase: phase.name,
      phaseClass: phase.cls,
      countdown: phase.sec
    });
    this.runPhase();
  },

  stopGuide() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
    this.setData({
      running: false,
      finished: false,
      round: 0,
      phase: '',
      phaseClass: '',
      countdown: 0
    });
  },

  onUnload() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
  }
});
