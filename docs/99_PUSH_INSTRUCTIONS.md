# 推送说明（GitHub 不可达时的操作步骤）

归档工作已全部完成，本地仓库 `26-TI-cup/` 已有 **3 次提交、275 个文件**，工作区干净。
唯一未完成的是 `git push` —— 因为当前环境到 GitHub 的网络被阻断。

---

## 为什么推不上去

实测结果：

| 目标 | 结果 |
|---|---|
| `github.com:443`（HTTPS）| ❌ **连接被重置**（`curl 28 Recv failure`）|
| `github.com:22`（SSH）| ✅ TCP 可达 |
| `ssh.github.com:443`（SSH over 443）| ✅ TCP 可达 |
| `gitee.com:443` | ✅ TCP 可达 |

即 **GitHub 的 443 端口被网络阻断，22 端口可达**。同时本机 **没有配置 GitHub 凭据助手**（`credential.helper` 为空），所以 HTTPS 方式也无法完成认证。

结论：**改用 SSH 推送**（走 22 端口，正好可达）。

---

## 操作步骤

### 1. 生成 SSH 密钥

在**你自己的终端**（PowerShell，不要经过任何沙箱）执行：

```powershell
ssh-keygen -t ed25519 -C "2567146709@qq.com"
```

一路回车即可（密钥路径默认 `C:\Users\Anlius\.ssh\id_ed25519`，密码可留空）。

> 我尝试过代你生成，但沙箱不允许写 `C:\Users\Anlius\.ssh\`；而且把私钥放进工作区并不安全，所以这一步留给你。

### 2. 把公钥添加到 GitHub

```powershell
Get-Content "$env:USERPROFILE\.ssh\id_ed25519.pub" | Set-Clipboard
```

然后打开 <https://github.com/settings/keys> → `New SSH key` → 粘贴 → 保存。

### 3. 验证 SSH 认证

```powershell
ssh -T git@github.com
```

看到 `Hi Anlius2024! You've successfully authenticated...` 即成功（提示 `does not provide shell access` 是正常的）。

### 4. 切换 remote 并推送

```powershell
cd E:\college\2nd_spring\26zhuan_xiang_sai\26-TI-cup
git remote set-url origin git@github.com:Anlius2024/26-TI-cup.git
git push -u origin main
```

### 5.（可选）若 22 端口也被封

改用 443 端口的 SSH：

```powershell
ssh -T -p 443 git@ssh.github.com
```

若这一步能通，在 `C:\Users\Anlius\.ssh\config` 中写入：

```
Host github.com
    HostName ssh.github.com
    Port 443
    User git
```

之后再执行第 4 步的 `git push`。

### 6.（备选）走代理

如果你有可用代理（例如本地 `7890` 端口）：

```powershell
git config --global http.proxy http://127.0.0.1:7890
git config --global https.proxy http://127.0.0.1:7890
git push -u origin main
```

---

## 推送后建议核对

1. 打开 <https://github.com/Anlius2024/26-TI-cup> 确认 275 个文件已上线
2. **重点看 README.md 的中文是否正常显示**（原文件为 UTF-8，应无乱码）
3. 确认 `docs/00_PROBLEM_2026.md` 中的赛题文字完整
4. 确认 `hardware/04_wien_bridge/文氏桥教程.pdf`（2.12 MB）已上传
5. 确认没有 `Debug/`、`.o`、`.out` 等编译产物混入

---

## 推送前已验证的检查项

在本地提交前已执行以下校验，**全部通过**：

| 检查 | 结果 |
|---|---|
| 厂商联系方式残留（手机号/邮箱/QQ）| ✅ 无 |
| 损坏的替换字符 `U+FFFD` | ✅ 无 |
| 本机绝对路径（`E:\college`、`C:\ti`）| ✅ 无 |
| 编译产物（`.o`/`.out`/`.map`/`.idx`）| ✅ 无 |
| `Debug/` 目录 | ✅ 无 |
| `.clangd`（自动生成）| ✅ 无 |
| Multisim `(Security copy)` 副本 | ✅ 无 |
| 源码括号配平（56 个 .c + 53 个 .h）| ✅ 0 个结构问题 |
| `targetConfigs/MSPM0G3507.ccxml` 保留 | ✅ 已保留 |

详细整理内容见 [`docs/90_THIRD_PARTY_NOTICES.md`](90_THIRD_PARTY_NOTICES.md)。
