// pages/index/index.js
Page({
  data: {},

  gotoTimer() {
    wx.navigateTo({ url: '/pages/sleep-timer/sleep-timer' });
  },
  gotoBreath() {
    wx.navigateTo({ url: '/pages/breath-guide/breath-guide' });
  },
  gotoCalc() {
    wx.navigateTo({ url: '/pages/sleep-calc/sleep-calc' });
  },
  gotoRecords() {
    wx.navigateTo({ url: '/pages/records/records' });
  }
});
