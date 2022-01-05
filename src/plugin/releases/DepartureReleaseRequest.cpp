#include "DepartureReleaseRequest.h"
#include "time/SystemClock.h"

namespace UKControllerPlugin::Releases {

    DepartureReleaseRequest::DepartureReleaseRequest(
        int id,
        std::string callsign,
        int requestingController,
        int targetController,
        std::chrono::system_clock::time_point requestExpiresAt)
        : id(id), callsign(std::move(callsign)), requestingController(requestingController),
          targetController(targetController), requestExpiresAt(requestExpiresAt), createdAt(Time::TimeNow())
    {
    }

    void DepartureReleaseRequest::Acknowledge()
    {
        this->acknowledgedAtTime = Time::TimeNow();
    }

    void DepartureReleaseRequest::Reject(std::string remarks)
    {
        this->rejectedAtTime = Time::TimeNow();
        this->remarks = remarks;
    }

    void DepartureReleaseRequest::Approve(
        std::chrono::system_clock::time_point releasedAtTime,
        std::chrono::system_clock::time_point releaseExpiresAt,
        std::string remarks)
    {
        this->releasedAtTime = releasedAtTime;
        this->releaseExpiresAt = releaseExpiresAt;
        this->remarks = remarks;
    }

    void DepartureReleaseRequest::Approve(std::chrono::system_clock::time_point releasedAtTime, std::string remarks)
    {
        this->releasedAtTime = releasedAtTime;
        this->remarks = remarks;
    }

    auto DepartureReleaseRequest::Id() const -> int
    {
        return this->id;
    }

    bool DepartureReleaseRequest::RequiresDecision() const
    {
        return !this->Rejected() && !this->RequestExpired() && !this->Approved();
    }

    auto DepartureReleaseRequest::Callsign() const -> std::string
    {
        return this->callsign;
    }

    auto DepartureReleaseRequest::RequestingController() const -> int
    {
        return this->requestingController;
    }

    auto DepartureReleaseRequest::TargetController() const -> int
    {
        return this->targetController;
    }

    bool DepartureReleaseRequest::Acknowledged() const
    {
        return this->acknowledgedAtTime != this->noTime;
    }

    auto DepartureReleaseRequest::Rejected() const -> bool
    {
        return this->rejectedAtTime != this->noTime;
    }

    auto DepartureReleaseRequest::Approved() const -> bool
    {
        return this->releasedAtTime != this->noTime;
    }

    auto DepartureReleaseRequest::RequestExpired() const -> bool
    {
        return this->requestExpiresAt < Time::TimeNow();
    }

    auto DepartureReleaseRequest::ApprovalExpired() const -> bool
    {
        return this->releaseExpiresAt < Time::TimeNow();
    }

    auto DepartureReleaseRequest::AwaitingReleasedTime() const -> bool
    {
        return this->releasedAtTime > Time::TimeNow();
    }

    auto DepartureReleaseRequest::ApprovedWithNoExpiry() const -> bool
    {
        return this->Approved() && this->releaseExpiresAt == this->noTimeMax;
    }

    auto DepartureReleaseRequest::RequestExpiryTime() const -> std::chrono::system_clock::time_point
    {
        return this->requestExpiresAt;
    }

    auto DepartureReleaseRequest::ReleaseExpiryTime() const -> std::chrono::system_clock::time_point
    {
        return this->releaseExpiresAt;
    }

    auto DepartureReleaseRequest::ReleasedAtTime() const -> std::chrono::system_clock::time_point
    {
        return this->releasedAtTime;
    }

    auto DepartureReleaseRequest::RejectedAtTime() const -> std::chrono::system_clock::time_point
    {
        return this->rejectedAtTime;
    }

    auto DepartureReleaseRequest::AcknowledgedAtTime() const -> std::chrono::system_clock::time_point
    {
        return this->acknowledgedAtTime;
    }

    auto DepartureReleaseRequest::CreatedAt() const -> std::chrono::system_clock::time_point
    {
        return this->createdAt;
    }

    auto DepartureReleaseRequest::Remarks() const -> const std::string&
    {
        return this->remarks;
    }
} // namespace UKControllerPlugin::Releases
