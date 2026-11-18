/* ============================================================
 * 云荟睡眠助手 - 控制台版
 * 依赖：仅 C 标准库 (stdio.h / stdlib.h / string.h / time.h)
 * 平台：Windows / Linux 均可编译运行
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---- 跨平台兼容宏 ---- */
#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN() system("cls")
    #define SLEEP_MS(ms)   Sleep(ms)
#else
    #include <unistd.h>
    #define CLEAR_SCREEN() system("clear")
    #define SLEEP_MS(ms)   usleep((ms) * 1000)
#endif

#define RECORD_FILE "sleep_record.txt"
#define MAX_LINE    256

/* 等待用户按回车键 */
static void wait_enter(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

/* 清空输入缓冲区 */
static void flush_input(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

/* 将 time_t 格式化为 "YYYY-MM-DD HH:MM" */
static void format_datetime(time_t t, char *buf, int size)
{
    struct tm *lt = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M", lt);
}

/* 判断输入是否为 'q' 退出指令（行首字符） */
static int is_quit_input(void)
{
    int ch = getchar();
    if (ch == 'q' || ch == 'Q') {
        while (getchar() != '\n' && !feof(stdin)) { }
        return 1;
    }
    /* 若不是 q，则把整行剩余字符消耗掉（含回车） */
    while (ch != '\n' && ch != EOF) { ch = getchar(); }
    return 0;
}

/* ---- 功能1：睡眠计时记录 ---- */
static void start_sleep_timer(void)
{
    time_t start_t, end_t;
    char start_buf[MAX_LINE], end_buf[MAX_LINE];
    int total_min, hours, minutes;
    const char *eval;

    printf("\n按回车键开始记录睡眠时长\n");
    wait_enter();

    start_t = time(NULL);
    CLEAR_SCREEN();
    printf("睡眠计时中... 按回车键结束计时\n\n");
    wait_enter();
    end_t = time(NULL);

    total_min = (int)((end_t - start_t) / 60);
    if (total_min < 1) total_min = 1;   /* 至少1分钟，避免显示0 */

    hours   = total_min / 60;
    minutes = total_min % 60;

    format_datetime(start_t, start_buf, sizeof(start_buf));
    format_datetime(end_t,   end_buf,   sizeof(end_buf));

    if (total_min >= 420) {       /* >=7小时=420分钟 */
        eval = "睡眠充足，状态很棒";
    } else if (total_min >= 300) { /* 5~7小时 */
        eval = "睡眠尚可，注意休息";
    } else {
        eval = "睡眠不足，建议补觉";
    }

    printf("入睡时间：%s\n", start_buf);
    printf("醒来时间：%s\n", end_buf);
    printf("本次睡眠时长：%d小时%d分钟\n", hours, minutes);
    printf("睡眠评价：%s\n", eval);

    /* 追加写入记录文件 */
    {
        char date_buf[16];
        FILE *fp = fopen(RECORD_FILE, "a");
        if (fp == NULL) {
            printf("记录保存失败，请检查文件权限\n");
        } else {
            strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", localtime(&start_t));
            fprintf(fp, "%s,%s,%s,%d\n", date_buf, start_buf, end_buf, total_min);
            fclose(fp);
        }
    }

    printf("\n按回车键返回主菜单\n");
    wait_enter();
}

/* ---- 功能2：478呼吸助眠引导 ---- */
static void breath_countdown(int seconds)
{
    int i;
    for (i = seconds; i >= 1; i--) {
        printf("%d ", i);
        fflush(stdout);
        SLEEP_MS(1000);
    }
    printf("\n");
}

static void breath_guide(void)
{
    int round = 1;

    printf("\n478呼吸法：吸气4秒 → 屏息7秒 → 呼气8秒，循环进行，");
    printf("帮助快速平复情绪、辅助入睡\n");
    printf("按回车键开始引导，输入q并回车可随时退出\n");
    wait_enter();

    while (1) {
        printf("\n▶ 第%d轮 - 吸气（4秒）\n", round);
        breath_countdown(4);

        printf("▶ 屏息（7秒）\n");
        breath_countdown(7);

        printf("▶ 呼气（8秒）\n");
        breath_countdown(8);

        printf("✅ 第%d轮呼吸完成\n", round);
        printf("\n（输入q退出，回车继续下一轮）\n");

        if (is_quit_input()) {
            break;
        }
        round++;
    }
}

/* ---- 功能3：入睡时间推算 ---- */
static void calc_sleep_time(void)
{
    int hour, min, ret;
    const int cycles[] = {5, 4, 3, 2};   /* 周期数，从多到少 */
    int ncycles = (int)(sizeof(cycles) / sizeof(cycles[0]));
    int i;

    while (1) {
        printf("\n请输入起床时间（时 分，示例：7 30）：");
        ret = scanf("%d %d", &hour, &min);
        flush_input();

        if (ret != 2 || hour < 0 || hour > 23 || min < 0 || min > 59) {
            printf("时间格式错误，请重新输入\n");
            continue;
        }
        break;
    }

    printf("\n若您计划 %02d:%02d 起床，推荐以下入睡时间（按睡眠周期倒推）：\n",
           hour, min);

    for (i = 0; i < ncycles; i++) {
        int cyc = cycles[i];
        int back_min = cyc * 90;          /* 每周期90分钟 */
        int wake_total = hour * 60 + min;
        int sleep_total = wake_total - back_min;

        /* 处理跨天：若推算结果为负，加上一天的分钟数 */
        if (sleep_total < 0) {
            sleep_total += 24 * 60;
        }
        int sh = sleep_total / 60;
        int sm = sleep_total % 60;

        float hours = (float)cyc * 1.5f;
        printf("睡%d个周期（%.1f小时）：推荐入睡 %02d:%02d\n",
               cyc, hours, sh, sm);
    }

    printf("\n建议选择睡4-5个周期，睡眠质量更佳\n");
    printf("\n按回车键返回主菜单\n");
    wait_enter();
}

/* ---- 功能4：查看睡眠记录 ---- */
static void show_records(void)
{
    FILE *fp;
    char line[MAX_LINE];
    int count = 0;
    int total_min = 0;

    fp = fopen(RECORD_FILE, "r");
    if (fp == NULL) {
        printf("\n暂无睡眠记录，快去开始第一次睡眠记录吧~\n");
        printf("\n按回车键返回主菜单\n");
        wait_enter();
        return;
    }

    /* 检查文件是否为空 */
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("\n暂无睡眠记录，快去开始第一次睡眠记录吧~\n");
        fclose(fp);
        printf("\n按回车键返回主菜单\n");
        wait_enter();
        return;
    }

    printf("\n%-12s %-12s %-12s %-10s %s\n",
           "日期", "入睡时间", "醒来时间", "睡眠时长", "状态");
    printf("------------------------------------------------------------\n");

    do {
        char date[32], start_t[32], end_t[32];
        int mins, h, m;
        const char *status;

        /* 解析：日期,入睡时间,醒来时间,分钟数 */
        if (sscanf(line, "%31[^,],%31[^,],%31[^,],%d",
                   date, start_t, end_t, &mins) == 4) {
            h = mins / 60;
            m = mins % 60;
            if (mins >= 420) {
                status = "充足";
            } else if (mins >= 300) {
                status = "尚可";
            } else {
                status = "不足";
            }
            printf("%-12s %-12s %-12s %02d小时%02d分  %s\n",
                   date, start_t + 11, end_t + 11, h, m, status);
            count++;
            total_min += mins;
        }
    } while (fgets(line, sizeof(line), fp) != NULL);

    fclose(fp);

    if (count == 0) {
        printf("\n暂无睡眠记录，快去开始第一次睡眠记录吧~\n");
    } else {
        int avg = total_min / count;
        printf("------------------------------------------------------------\n");
        printf("共%d条记录，平均睡眠时长：%d小时%d分钟\n",
               count, avg / 60, avg % 60);
    }

    printf("\n按回车键返回主菜单\n");
    wait_enter();
}

/* ---- 主菜单 ---- */
static void show_menu(void)
{
    CLEAR_SCREEN();
    printf("========== 云荟睡眠助手 ==========\n");
    printf("科学作息，安睡每一夜\n\n");
    printf("1. 开始睡眠计时\n");
    printf("2. 478呼吸助眠引导\n");
    printf("3. 入睡时间推算\n");
    printf("4. 查看睡眠记录\n");
    printf("5. 退出程序\n");
    printf("请选择功能（1-5）：");
}

int main(void)
{
    int choice;

    while (1) {
        show_menu();
        if (scanf("%d", &choice) != 1) {
            flush_input();
            printf("输入无效，请输入1-5的数字重新选择\n");
            SLEEP_MS(1500);
            continue;
        }
        flush_input();

        switch (choice) {
            case 1: start_sleep_timer(); break;
            case 2: breath_guide();      break;
            case 3: calc_sleep_time();   break;
            case 4: show_records();      break;
            case 5:
                printf("\n感谢使用云荟睡眠助手，祝您晚安~\n");
                return 0;
            default:
                printf("输入无效，请输入1-5的数字重新选择\n");
                SLEEP_MS(1500);
                break;
        }
    }
    return 0;
}
