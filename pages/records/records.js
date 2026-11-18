// pages/records/records.js
const app = getApp();

Page({
  data: {
    records: [],
    avgText: '',
    hasRecords: false
  },

  onShow() {
    this.loadRecords();
  },

  loadRecords() {
    const raw = (app.globalData.records || []).slice();
    // 按日期倒序排列（最新的在前）
    raw.reverse();

    if (raw.length === 0) {
      this.setData({ hasRecords: false, records: [], avgText: '' });
      return;
    }

    let totalMin = 0;
    // 预先格式化，避免在 WXML 中使用不支持的表达式
    const records = raw.map(r => {
      totalMin += r.minutes;
      const h = Math.floor(r.minutes / 60);
      const m = r.minutes % 60;
      let durationText = '';
      if (h > 0) durationText += h + '小时';
      durationText += m + '分钟';

      let statusText = '不足';
      let statusClass = 'bad';
      if (r.minutes >= 420) {
        statusText = '充足';
        statusClass = 'good';
      } else if (r.minutes >= 300) {
        statusText = '尚可';
        statusClass = 'ok';
      }

      return {
        date: r.date,
        start: r.start,
        end: r.end,
        minutes: r.minutes,
        durationText: durationText,
        statusText: statusText,
        statusClass: statusClass
      };
    });

    const avg = Math.floor(totalMin / records.length);
    const avgText = '共' + records.length + '条记录，平均睡眠 ' + Math.floor(avg / 60) + '小时' + (avg % 60) + '分钟';

    this.setData({
      records: records,
      avgText: avgText,
      hasRecords: true
    });
  },

  clearRecords() {
    wx.showModal({
      title: '确认清空',
      content: '将删除所有睡眠记录，不可恢复',
      confirmColor: '#e94560',
      success: (res) => {
        if (res.confirm) {
          app.globalData.records = [];
          wx.removeStorageSync('sleep_records');
          this.setData({ hasRecords: false, records: [], avgText: '' });
        }
      }
    });
  }
});
