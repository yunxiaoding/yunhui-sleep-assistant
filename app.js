// app.js
App({
  globalData: {
    // 睡眠记录存储在本地缓存，key 与 C 版的 sleep_record.txt 对应
    records: []
  },

  onLaunch() {
    // 启动时从本地缓存加载历史记录
    this.globalData.records = wx.getStorageSync('sleep_records') || [];
  },

  // 保存一条睡眠记录
  addRecord(record) {
    this.globalData.records.push(record);
    wx.setStorageSync('sleep_records', this.globalData.records);
  }
});
