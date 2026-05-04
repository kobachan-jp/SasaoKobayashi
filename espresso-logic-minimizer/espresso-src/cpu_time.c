#include <windows.h>

/* Espresso内部で使われる関数名に合わせて定義 */
long util_cpu_time(void)
{
    FILETIME createTime, exitTime, kernelTime, userTime;
    /* Windows APIを使用してプロセスの実行時間を取得 */
    if (GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER kernel, user;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;
        
        /* 100ナノ秒単位をミリ秒単位に変換 (10000で割る) */
        /* ユーザー時間とシステム時間の合計を返します */
        return (long)((kernel.QuadPart + user.QuadPart) / 10000);
    }
    return 0;
}

/* 互換性のために cpu_time という名前でも呼べるようにしておく */
long cpu_time(void) 
{
    return util_cpu_time();
}
