#include "DepartureReleaseRequest.h"
#include "controller/ControllerPosition.h"
#include "time/SystemClock.h"

namespace UKControllerPlugin::Releases {

    DepartureReleaseRequest::DepartureReleaseRequest(
        int id,
        std::string callsign,
        const std::shared_ptr<Controller::ControllerPosition>& requestingController,
        const std::shared_ptr<Controller::ControllerPosition>& targetController,
        std::chrono::system_clock::time_point requestExpiresAt)
        : id(id), callsign(std::move(callsign)), requestingController(requestingController),
          targetController(targetController), requestExpiresAt(requestExpiresAt), createdAt(Time::TimeNow())
    {
        assert(requestingController && "requesting controller is nullptr");
        assert(targetController && "target controller is nullptr");
    }

    void DepartureReleaseRequest::Acknowledge()
    {
        this->acknowledgedAtTime = Time::TimeNow();
    }

    void DepartureReleaseRequest::Reject(std::string remarks)
    {
        this->rejectedAtTime = Time::TimeNow();
        this->remarks = std::move(remarks);
    }

    void DepartureReleaseRequest::Approve(
        std::chrono::system_clock::time_point releasedAtTime,
        std::chrono::system_clock::time_point releaseExpiresAt,
        std::string remarks)
    {
        this->releasedAtTime = releasedAtTime;
        this->releaseExpiresAt = releaseExpiresAt;
        this->remarks = std::move(remarks);
    }

    void DepartureReleaseRequest::Approve(std::chrono::system_clock::time_point releasedAtTime, std::string remarks)
    {
        this->releasedAtTime = releasedAtTime;
        this->remarks = std::move(remarks);
    }

    auto DepartureReleaseRequest::Id() const -> int
    {
        return this->id;
    }

    auto DepartureReleaseRequest::RequiresDecision() const -> bool
    {
        return !this->Rejected() && !this->RequestExpired() && !this->Approved();
    }

    auto DepartureReleaseRequest::Callsign() const -> const std::string&
    {
        return this->callsign;
    }

    auto DepartureReleaseRequest::RequestingController() const -> std::shared_ptr<Controller::ControllerPosition>
    {
        return this->requestingController;
    }

    auto DepartureReleaseRequest::RequestingControllerId() const -> int
    {
        return this->requestingController->GetId();
    }

    auto DepartureReleaseRequest::TargetController() const -> std::shared_ptr<Controller::ControllerPosition>
    {
        return this->targetController;
    }

    auto DepartureReleaseRequest::TargetControllerId() const -> int
    {
        return this->targetController->GetId();
    }

    auto DepartureReleaseRequest::Acknowledged() const -> bool
    {
        return this->acknowledgedAtTime != DepartureReleaseRequest::noTime;
    }

    auto DepartureReleaseRequest::Rejected() const -> bool
    {
        return this->rejectedAtTime != DepartureReleaseRequest::noTime;
    }

    auto DepartureReleaseRequest::Approved() const -> bool
    {
        return this->releasedAtTime != DepartureReleaseRequest::noTime;
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
        return this->Approved() && this->releaseExpiresAt == DepartureReleaseRequest::noTimeMax;
    }

    auto DepartureReleaseRequest::RequestExpiryTime() const -> const std::chrono::system_clock::time_point&
    {
        return this->requestExpiresAt;
    }

    auto DepartureReleaseRequest::ReleaseExpiryTime() const -> const std::chrono::system_clock::time_point&
    {
        return this->releaseExpiresAt;
    }

    auto DepartureReleaseRequest::ReleasedAtTime() const -> const std::chrono::system_clock::time_point&
    {
        return this->releasedAtTime;
    }

    auto DepartureReleaseRequest::RejectedAtTime() const -> const std::chrono::system_clock::time_point&
    {
        return this->rejectedAtTime;
    }

    auto DepartureReleaseRequest::AcknowledgedAtTime() const -> const std::chrono::system_clock::time_point&
    {
        return this->acknowledgedAtTime;
    }

    auto DepartureReleaseRequest::CreatedAt() const -> const std::chrono::system_clock::time_point&
    {
        return this->createdAt;
    }

    auto DepartureReleaseRequest::Remarks() const -> const std::string&
    {
        return this->remarks;
    }
} // namespace UKControllerPlugin::Releases
