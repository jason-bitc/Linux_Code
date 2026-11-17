#!/usr/bin/env bash
# Install the GCC header fix once for all clangd projects on this Linux host.
set -euo pipefail

gcc_runtime=$(g++ -print-file-name=crtbegin.o)
if [[ "$gcc_runtime" != /* || ! -f "$gcc_runtime" ]]; then
    echo '无法定位 g++ 的 GCC 安装目录，请先安装 g++。' >&2
    exit 1
fi
gcc_dir=$(dirname -- "$gcc_runtime")
config_dir="${XDG_CONFIG_HOME:-$HOME/.config}/clangd"
config_file="$config_dir/config.yaml"
config_flag="  Add: [--gcc-install-dir=$gcc_dir]"

mkdir -p -- "$config_dir"
if [[ -f "$config_file" ]] && grep -Fqx -- "$config_flag" "$config_file"; then
    printf '配置已存在：%s\n' "$config_file"
    exit 0
fi

if [[ -s "$config_file" ]]; then
    backup_file="${config_file}.bak-$(date +%Y%m%d-%H%M%S-%N)"
    cp -p -- "$config_file" "$backup_file"
    printf '\n---\n' >> "$config_file"
    printf '原配置已备份：%s\n' "$backup_file"
fi

{
    printf '# Match clangd to the GCC installation used by g++.\n'
    printf 'CompileFlags:\n%s\n' "$config_flag"
} >> "$config_file"

printf '已安装全局 clangd 配置：%s\n' "$config_file"
printf 'GCC 安装目录：%s\n' "$gcc_dir"
printf '在 VS Code 命令面板执行 clangd: Restart language server 后生效。\n'
