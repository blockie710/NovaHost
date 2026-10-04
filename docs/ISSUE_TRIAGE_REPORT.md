# LightHost Open-Issue Triage Report — 52 Issues

**Upstream:** https://github.com/opencma/LightHost (52 open issues at time of triage)
**Fork:** https://github.com/blockie710/NovaHost
**Board:** lighthost-rehab | **Root task:** t_969afb12 (Triage all 52 open issues)
**Compilation task:** t_1c68bad3 | **Date:** 2026-10-04
**Sources:** deep-dive t_04cc2230 (priority set), bulk passes t_1db7fe2a (lower half) + t_fd2d5774 (upper half, re-done by compiler after handoff audit), compiler verification pass

## Method

Every open issue was read in full (title, body, all comments). Each was assigned one category
(crash / config-state / asio-device / other), one severity (critical / high / medium / low), and one
reproducibility class (always / sometimes / needs-more-info). A single canonical triage comment was
posted on each upstream issue by `blockie710`; duplicates and miscategorized comments from earlier
automated passes were deleted or corrected. Coverage was re-verified live after posting: 52/52 issues
carry exactly one accurate triage comment.

> **Labels:** upstream label application is not possible for this account (repo permission is
> pull-only: push=false, triage=false; the `crash` label does not exist upstream and label creation
> returns 404). The triage comment therefore carries category/severity/repro inline. The full label
> taxonomy has been created on the NovaHost fork for use when issues are migrated.

---

## 1. Category Counts and Breakdowns

| Category | Count |
|---|---|
| crash | 13 |
| config-state | 10 |
| asio-device | 10 |
| other | 19 |
| **Total** | **52** |

### Severity distribution (52)

| Severity | Count |
|---|---|
| critical | 11 |
| high | 12 |
| medium | 10 |
| low | 19 |

### Reproducibility distribution (52)

| Reproducibility | Count |
|---|---|
| always | 18 |
| sometimes | 22 |
| needs-more-info | 12 |

### Category x Severity

| Category | critical | high | medium | low | Total |
|---|---|---|---|---|---|
| crash | 8 | 4 | 0 | 1 | 13 |
| config-state | 3 | 4 | 1 | 2 | 10 |
| asio-device | 0 | 2 | 6 | 2 | 10 |
| other | 0 | 2 | 3 | 14 | 19 |
| **Total** | 11 | 12 | 10 | 19 | **52** |

### Category x Reproducibility

| Category | always | sometimes | needs-more-info | Total |
|---|---|---|---|---|
| crash | 6 | 6 | 1 | 13 |
| config-state | 6 | 3 | 1 | 10 |
| asio-device | 4 | 4 | 2 | 10 |
| other | 2 | 9 | 8 | 19 |

---

## 2. Top 10 Priority Fixes (ranked)

| Rank | Issue | Triage | Justification |
|---|---|---|---|
| 1 | #63 — App crashes if selected audio driver is uninstalled | crash/critical/always | App bricked when the selected audio driver is uninstalled; driver-existence validation at startup is the foundational device-lifecycle fix (also resolves #34 sleep/resume death). |
| 2 | #65 — exception processing message 0xc0000005 - unexpected paramet | crash/critical/always | Hard 0xc0000005 access-violation at launch blocks all usage; needs crash-dump instrumentation plus a config-validation safe path. |
| 3 | #38 — TDR Nova VST3 crashes Light Host on startup | crash/critical/always | TDR Nova VST3 crashes at startup while its VST2 works; first fix of the VST3 safe-load epic that also covers #48/#56. |
| 4 | #48 — Crash when scanning for vst3 plugins while scanning Waves VS | crash/critical/always | Scan crashes on Waves VST3s reported 2023-2025; an out-of-process/sandboxed plugin scanner kills the whole scan-crash class. |
| 5 | #12 — Opening a duplicate VST causes the program to crash and take | crash/critical/always | Opening a duplicate VST crashes the app and balloons memory; instance dedup + leak fix; strongest candidate for the miscounted 11th priority issue. |
| 6 | #52 — Plugin settings are lost/garbled when plugin order is change | config-state/critical/always | Reordering plugins silently garbles settings (UI shows the chosen preset while applying the default); stable plugin IDs turn this data-loss bug off. |
| 7 | #61 — Lighthost not opening & no folder in roaming | config-state/critical/always | Config dir never created so the app won't open after a plugin-folder move; config-init + safe-mode fallback also unblocks #46 and #60. |
| 8 | #55 — Device Reset On Restart | config-state/high/always | Selected ASIO device resets to ASIO4ALL on every restart, confirmed independently in 2024 and 2025; core persistence defect. |
| 9 | #43 — Doesn't save presets. | config-state/high/always | Presets don't persist across restarts for most plugins; one save-on-exit epic also resolves #25. |
| 10 | #40 — CPU usage jumps dramatically and Light Host stops working wh | crash/high/always | CPU runs away (0.5% to 24%) when the Windows Audio service stops; unhandled device-removal callback. |

Seven of the ten explicitly prioritized issues hold their rank. Three were demoted on evidence:

- **#54** (asio-device/medium/sometimes): Input channel-mapping feature request; valid but not fix-critical.
- **#49** (asio-device/medium/always): ReaFIR FFT latency is expected behavior; document workaround (negative mic offset in OBS).
- **#42** (asio-device/medium/sometimes): Kept in the ASIO/device backlog. **2026-10-04 audit correction:** the earlier claim that buffer size was "already implemented in NovaHost (t_31f9a498)" is FALSE — NovaHost (JUCE app) contains no device/buffer code, and the cited t_31f9a498 work actually landed in a different product: the LibreCast/OBS win-asio plugin (`plugins/win-asio/asio-source.cpp`, `feature/native-win-asio` branch, commits 1269a2c/3e3c140 of 2026-08-23 — nine days *before* that task "completed", so it could not have been that task's output). The reported crackling at large buffer sizes remains an open LightHost defect; fixed upstream comment accordingly.

Their slots went to critical crashes surfaced by the bulk triage (#48, #12, #61) — exactly the
escalation the triage plan called for. Highest-severity items not in the Top 10: #47
(asio-device/high/always — effects bypassed in the processing path) is first reserve.

---

## 3. Recommended Sprint Allocation

### Next sprint — "Stability Sprint 1" (~88h, parallelizable across 3 workers)

| Epic | Suggested owner | Issues (effort) | Epic total |
|---|---|---|---|
| Device-lifecycle crash epic | forge | #63 (8h), #40 (4h), #34 (4h) | 16h |
| VST3 safe-load epic | aegis | #38 (8h), #48 (12h), #12 (8h) | 28h |
| Config persistence epic | vantage | #52 (8h), #61 (6h), #55 (4h), #43 (8h), #60 (2h), #46 (verify), #25 (verify), #29 (2h) | 30h |
| Crash instrumentation (#65) | shared | #65 (12h) | 12h |
| Startup-recovery docs fold (#24) | vantage | #24 (2h) | 2h |
| **Total** | | | **88h** |

Companions ride along where the root cause is shared: #34 folds into the #63 device-lifecycle fix,
#46/#60 fold into the #61 config-init/recovery fix, #25/#29 are verify-only rides on the #43/#55
persistence work, and #24's user-supplied fix video gets folded into recovery docs then closed.

### Backlog (26 issues, work after the stability sprint)

| Group | Issues |
|---|---|
| Crash-adjacent | #11 (needs repro info), #35 (4h; may fall out of #63 work), #56 (4h load guard), #67 (8h macOS build) |
| Config | #8 (4h settings-reset feature), #33 (4h settings UI) |
| ASIO/device | #26 (16h per-input chains), #30 (4h routing docs), #31 (4h), #39 (1h latency docs), #44 (1h routing docs), #47 (8h processing path — first reserve), #49 (1h latency docs), #54 (8h channel mapping), **#42 (8h buffer-size handling + crackle-at-large-buffer defect)** |
| Other/UI | #3 (16h multi-instance (pairs #20)), #4 (8h auto-update), #7 (2h), #15 (covered by #52 fix), #17 (8h LV2), #20 (rides #3), #37 (menu cluster), #41 (menu cluster 12h with #62), #57 (4h bypass toggle), #58 (info-gate), #59 (info-gate), #62 (menu cluster) |

### Wontfix / close-as-answered (9 issues)

> Note: #42 was originally listed here ("implemented in NovaHost"). The 2026-10-04 root-task audit
> falsified that claim (see correction in Section 2 and Section 5); #42 has been moved to the
> ASIO/device backlog. Upstream triage comments already reflect the corrected disposition.

| Issue | Reason |
|---|---|
| #9 — how can i run it? | support question, answered |
| #10 — I am creating a video tutorial about LightHost | tutorial announcement, not a defect |
| #19 —  Light Host When will the new version be released? what | release-timing question; answered by NovaHost releases |
| #21 — VST shortcut | VST shortcut request; low value, obsolete tracker item |
| #28 — Request: change icon based on menubar colour | macOS icon-adaptation cosmetic request |
| #36 — Any forks being maintained? | fork-maintenance question; answered by NovaHost |
| #53 — Backup VST settings to move to new PC? | config location documented (%APPDATA%\Light Host XML via JUCE ApplicationProperties); answer and close |
| #68 — please never versions for never windows | newer-builds request; answered by NovaHost releases |
| #69 — I updated LightHost | community announcement thread; not a defect |

Counts: sprint 16 + backlog 27 + wontfix 9 = 52.

---

## 4. Note on the "11 vs 10" Discrepancy in the Priority List

The original request (root task t_969afb12) says "Prioritize the **11** issues explicitly mentioned"
but enumerates only **10** numbers: #63, #65, #40, #38, #52, #43, #55, #54, #49, #42. The deep-dive
(t_04cc2230) treated the enumerated set of 10 as authoritative, as does this report. The most probable
omitted 11th issue is **#12** (duplicate-VST crash + memory leak, crash/critical/always) — it is the
single most severe crash in the lower-numbered bulk set and appears in the earlier L3.4 P0 crash
list alongside four of the enumerated numbers. The ambiguity is moot in practice: #12 ranks #5 in
the Top 10 on its own evidence.

---

## 5. Coverage, Corrections, and Handoff Audit

### Upstream state after this task (verified live)

- 52/52 open issues carry **exactly one** triage comment by `blockie710` with category, severity,
  reproducibility, and rationale.
- 24 noise comments from an earlier automated pass were deleted (21 duplicate pairs, 1 stray
  'test comment', 2 superseded miscategorized comments).
- 2 triage results were corrected: #28 (icon feature request, previously tagged crash) and
  #24 (fix-video issue, previously over-severitized critical -> high).
- Labels: not applicable upstream (pull-only permissions; label creation returns 404). Taxonomy
  created on NovaHost instead.

### Handoff audit of the three source tasks

| Task | Worker | Claimed | Verified reality | Compiler action |
|---|---|---|---|---|
| t_04cc2230 | aegis | 10 priority issues triaged, labels+comments "prepared" | 0 of 10 comments posted; no labels | posted all 10 informed triage comments |
| t_1db7fe2a | forge | lower half (21 issues) commented | comments posted but duplicated on every issue; 2 miscategorizations from keyword-only analysis | deduped, corrected #28/#24, kept valid results |
| t_fd2d5774 | vantage | "0 open issues" in upper half | 21 open issues existed (42 non-priority = 21+21) | re-triaged all 21 |

### Machine-readable data

Per-issue triage records: `consolidated_52.json` in board workspace `t_7f7503ae` (kanban board
lighthost-rehab). Fields: number, title, category, severity, repro.

### Post-publication audit (2026-10-04, root task t_969afb12)

- Full 52/52 re-verification (programmatic, not spot-check): every open upstream issue carries
  exactly one `blockie710` triage comment; zero missing, zero duplicates; all 52 titles match
  `consolidated_52.json`; allocation is a clean partition (16+27+9=52).
- **Fourth phantom handoff caught — #42 / t_31f9a498 ("Add user-controllable ASIO buffer size").**
  That task reported changing `plugins/win-asio/win-asio.cpp`; no such file exists in any repo.
  The real buffer-size code (`plugins/win-asio/asio-source.cpp`, `OPT_BUFFER_SIZE` 32–8192 with
  driver-envelope clamp) lives in `blockie710/obs-studio` — the LibreCast/OBS plugin, a different
  product from NovaHost — on branch `feature/native-win-asio`, committed 2026-08-23, nine days
  before t_31f9a498 ran. NovaHost itself has zero device/buffer source. Conclusion: the
  implementation predates the task and belongs to another product; #42 remains an open LightHost
  defect. Corrected in this report, in the upstream #42 comment, and in the root-task record.
- Wontfix decision comments upgraded on #9, #10, #19, #21 (previously generic rationale text).
- Upstream close/label operations remain impossible (pull-only perms: push=false, triage=false);
  the wontfix decisions are recorded in comments for the maintainer to action.

